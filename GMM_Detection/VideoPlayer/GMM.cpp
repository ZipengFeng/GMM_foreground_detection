#include "StdAfx.h"
#include "GMM.h"

MOG_BGS::MOG_BGS(void)
{

}

MOG_BGS::~MOG_BGS(void)
{

}

// 全部初始化为0
void MOG_BGS::init(const Mat _image)
{
	//参数初始化
	for (int i = 0; i < GMM_MAX_COMPONT; i++)
	{
		m_weight[i] = Mat::zeros(_image.size(), CV_32FC1);
		m_mean[i] = Mat::zeros(_image.size(), CV_8UC1);
		m_sigma[i] = Mat::zeros(_image.size(), CV_32FC1);
	}
	m_mask = Mat::zeros(_image.size(), CV_8UC1);
	m_fit_num = Mat::ones(_image.size(), CV_8UC1);
}

//gmm第一帧初始化函数实现
//捕获到第一帧时对高斯分布进行初始化．主要包括对每个高斯分布的权值、期望和方差赋初值．
//其中第一个高斯分布的权值为1，期望为第一个像素数据．其余高斯分布权值为0，期望为0．
//每个高斯分布都被赋予适当的相等的初始方差 15
void MOG_BGS::processFirstFrame(const Mat _image)
{
	for (int i = 0; i < GMM_MAX_COMPONT; i++)
	{
		if (i == 0)
		{
			m_weight[i].setTo(1.0);
			_image.copyTo(m_mean[i]);
			m_sigma[i].setTo(15.0);
		}
		else
		{
			m_weight[i].setTo(0.0);
			m_mean[i].setTo(0);
			m_sigma[i].setTo(15.0);
		}
	}
}

// 通过新的帧来训练GMM
void MOG_BGS::trainGMM(const Mat _image)
{
	for (int i = 0; i < _image.rows; i++)
	{
		for (int j = 0; j < _image.cols; j++)
		{
			int num_fit = 0;

			for (int k = 0; k < GMM_MAX_COMPONT; k++)
			{
				int delm = abs(_image.at<uchar>(i, j) - m_mean[k].at<uchar>(i, j));
				long dist = delm * delm;
				// 判断是否匹配：采样值与高斯分布的均值的距离小于3倍方差（表示匹配）
				if (dist < 3.0 * m_sigma[k].at<float>(i, j))
				{
					// 如果匹配
					/****更新权重****/
					m_weight[k].at<float>(i, j) += GMM_LEARN_ALPHA * (1 - m_weight[k].at<float>(i, j));

					/****更新均值****/
					//m_mean[k].at<uchar>(i, j) += (GMM_LEARN_ALPHA /m_weight[k].at<uchar>(i, j)) * delm;
					m_mean[k].at<uchar>(i, j) += (uchar)(GMM_LEARN_ALPHA / m_weight[k].at<float>(i, j)) * delm;

					/****更新方差****/
					m_sigma[k].at<float>(i, j) += (GMM_LEARN_ALPHA / m_weight[k].at<float>(i, j)) * (dist - m_sigma[k].at<float>(i, j));
				}
				else
				{
					// 如果不匹配。则该该高斯模型的权值变小
					m_weight[k].at<float>(i, j) += GMM_LEARN_ALPHA * (0 - m_weight[k].at<float>(i, j));
					num_fit++; // 不匹配的模型个数
				}
			}

			//对gmm各个高斯进行排序,从大到小排序,排序依据为 weight / sigma
			for (int kk = 0; kk < GMM_MAX_COMPONT; kk++)
			{
				for (int rr = kk; rr< GMM_MAX_COMPONT; rr++)
				{
					if (m_weight[rr].at<float>(i, j) / m_sigma[rr].at<float>(i, j) > m_weight[kk].at<float>(i, j) / m_sigma[kk].at<float>(i, j))
					{
						//权值交换
						float temp_weight = m_weight[rr].at<float>(i, j);
						m_weight[rr].at<float>(i, j) = m_weight[kk].at<float>(i, j);
						m_weight[kk].at<float>(i, j) = temp_weight;

						//均值交换
						uchar temp_mean = m_mean[rr].at<uchar>(i, j);
						m_mean[rr].at<uchar>(i, j) = m_mean[kk].at<uchar>(i, j);
						m_mean[kk].at<uchar>(i, j) = temp_mean;

						//方差交换
						float temp_sigma = m_sigma[rr].at<float>(i, j);
						m_sigma[rr].at<float>(i, j) = m_sigma[kk].at<float>(i, j);
						m_sigma[kk].at<float>(i, j) = temp_sigma;
					}
				}
			}

			if (num_fit == GMM_MAX_COMPONT && 0 == m_weight[GMM_MAX_COMPONT - 1].at<float>(i, j))
			{
				//当有新值出现的时候，若目前分布个数小于M，新添一个分布，以新采样值作为均值，并赋予较大方差和较小权值
				for (int k = 0; k < GMM_MAX_COMPONT; k++)
				{
					if (0 == m_weight[k].at<float>(i, j))
					{
						m_weight[k].at<float>(i, j) = GMM_LEARN_ALPHA;
						m_mean[k].at<uchar>(i, j) = _image.at<uchar>(i, j);
						m_sigma[k].at<float>(i, j) = 15.0;

						for (int q = 0; q < GMM_MAX_COMPONT && q != k; q++)
						{
							//对其他的高斯模型的权值进行更新，保持权值和为1
							m_weight[q].at<float>(i, j) *= (1 - GMM_LEARN_ALPHA);
						}
						break; //找到第一个权值不为0的即可
					}
				}
			}
			else if (num_fit == GMM_MAX_COMPONT && m_weight[GMM_MAX_COMPONT - 1].at<float>(i, j) != 0)
			{
				//如果GMM_MAX_COMPONT都曾经赋值过，则用新来的高斯代替权值最弱的高斯，权值不变，只更新均值和方差
				m_mean[GMM_MAX_COMPONT - 1].at<uchar>(i, j) = _image.at<uchar>(i, j);
				m_sigma[GMM_MAX_COMPONT - 1].at<float>(i, j) = 15.0;
			}
		}
	}
}

//对输入图像每个像素gmm选择合适的高斯分量个数
//排序后最有可能是背景分布的排在最前面，较小可能的短暂的分布趋向于末端．我们将排序后的前fit_num个分布选为背景模型;
//在排过序的分布中，累积概率超过GMM_THRESHOD_SUMW的前fit_num个分布被当作背景模型，剩余的其它分布被当作前景模型．
void MOG_BGS::getFitNum(const Mat _image)
{
	for (int i = 0; i < _image.rows; i++)
	{
		for (int j = 0; j < _image.cols; j++)
		{
			float sum_w = 0.0;	//重新赋值为0，给下一个像素做累积
			for (uchar k = 0; k < GMM_MAX_COMPONT; k++)
			{
				sum_w += m_weight[k].at<float>(i, j);
				if (sum_w >= GMM_THRESHOD_SUMW)	//如果这里THRESHOD_SUMW=0.6的话，那么得到的高斯数目都为1，因为每个像素都有一个权值接近1
				{
					m_fit_num.at<uchar>(i, j) = k + 1;
					break;
				}
			}
		}
	}
}

//gmm测试函数的实现
void MOG_BGS::testGMM(const Mat _image)
{
	for (int i = 0; i < _image.rows; i++)
	{
		for (int j = 0; j < _image.cols; j++)
		{
			int k = 0;
			for (; k < m_fit_num.at<uchar>(i, j); k++)
			{
				if (abs(_image.at<uchar>(i, j) - m_mean[k].at<uchar>(i, j)) < (uchar)(2.5 * m_sigma[k].at<float>(i, j)))
				{
					m_mask.at<uchar>(i, j) = 0;
					break;
				}
			}
			if (k == m_fit_num.at<uchar>(i, j))
			{
				m_mask.at<uchar>(i, j) = 255;
			}
		}
	}
}

/*
Input:
src: 待检测连通域的二值化图像
Output:
dst: 标记后的图像
featherList: 连通域特征的清单
return：
连通域数量。
*/
int bwLabel(Mat & src, Mat & dst, vector<Feather> & featherList)
{
	int rows = src.rows;
	int cols = src.cols;

	int labelValue = 0;
	Point seed, neighbor;
	stack<Point> pointStack;    // 堆栈  

	int area = 0;               // 用于计算连通域的面积  
	int leftBoundary = 0;       // 连通域的左边界，即外接最小矩形的左边框，横坐标值，依此类推  
	int rightBoundary = 0;
	int topBoundary = 0;
	int bottomBoundary = 0;
	Rect box;                   // 外接矩形框  
	Feather feather;

	featherList.clear();    // 清除数组  

	dst.release();
	dst = src.clone();
	for (int i = 0; i < rows; i++)
	{
		uchar *pRow = dst.ptr<uchar>(i);
		for (int j = 0; j < cols; j++)
		{
			if (pRow[j] == 255)
			{
				area = 0;
				labelValue++;           // labelValue最大为254，最小为1.  
				seed = Point(j, i);     // Point（横坐标，纵坐标）  
				dst.at<uchar>(seed) = labelValue;
				pointStack.push(seed);

				area++;
				leftBoundary = seed.x;
				rightBoundary = seed.x;
				topBoundary = seed.y;
				bottomBoundary = seed.y;

				while (!pointStack.empty())
				{
					neighbor = Point(seed.x + 1, seed.y);
					if ((seed.x != (cols - 1)) && (dst.at<uchar>(neighbor) == 255))
					{
						dst.at<uchar>(neighbor) = labelValue;
						pointStack.push(neighbor);

						area++;
						if (rightBoundary < neighbor.x)
							rightBoundary = neighbor.x;
					}

					neighbor = Point(seed.x, seed.y + 1);
					if ((seed.y != (rows - 1)) && (dst.at<uchar>(neighbor) == 255))
					{
						dst.at<uchar>(neighbor) = labelValue;
						pointStack.push(neighbor);

						area++;
						if (bottomBoundary < neighbor.y)
							bottomBoundary = neighbor.y;

					}

					neighbor = Point(seed.x - 1, seed.y);
					if ((seed.x != 0) && (dst.at<uchar>(neighbor) == 255))
					{
						dst.at<uchar>(neighbor) = labelValue;
						pointStack.push(neighbor);

						area++;
						if (leftBoundary > neighbor.x)
							leftBoundary = neighbor.x;
					}

					neighbor = Point(seed.x, seed.y - 1);
					if ((seed.y != 0) && (dst.at<uchar>(neighbor) == 255))
					{
						dst.at<uchar>(neighbor) = labelValue;
						pointStack.push(neighbor);

						area++;
						if (topBoundary > neighbor.y)
							topBoundary = neighbor.y;
					}

					seed = pointStack.top();
					pointStack.pop();
				}
				box = Rect(leftBoundary, topBoundary, rightBoundary - leftBoundary, bottomBoundary - topBoundary);
				//rectangle(src, box, 255);
				feather.area = area;
				feather.boundingbox = box;
				feather.label = labelValue;
				// 删除掉过小的连通域
				if (feather.area >= 50)
				{
					featherList.push_back(feather);
				}

			}
		}
	}
	return labelValue;
}