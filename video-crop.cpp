/*
 * Видео содержит 2 вставки - 2 ряда кадров, содержащих белый прямоугольник на черном фоне - они являются метками.
 * В процессе обработки необходимо обнаружить метки и определить их позиционирование и размер в кадре.
 * В выходной файл поместить видеоданные, расположенные хронометрически между двумя метками.
 * Кадры выходного видео должны быть обрезаны относительно входного видео согласно белому прямоугольнику в метках.
 *
 * Входные данные : видео файл AVI, формат H264(yuv420p)
 * Выходные данные : видео файл AVI формат RAW(yuv420p)
*/

#include <iostream>
#include <string>

#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat)
#include <opencv2/videoio.hpp>  // Video write
#include <opencv2/imgproc.hpp> // Image processing

#define MARKER_THRESHOLD 100

using namespace std;
using namespace cv;

static void help(char* exec_name)
{
	cout
		<< "------------------------------------------------------------------------------" << endl
		<< "This program crops video files with black marker contained white crop-rectangle." << endl
		<< "Usage:" << endl
		<< exec_name << " <input_video_name>" << endl
		<< "------------------------------------------------------------------------------" << endl
		<< endl;
}

int main(int argc, char* argv[])
{
	help(argv[0]);
	if (argc < 2)
	{
		cout << "Not enough parameters" << endl;
		return -1;
	}

	const string source(argv[1]); // the source file name
	VideoCapture inputVideo(source); // Open input

	if (!inputVideo.isOpened())
	{
		cout << "Could not open the input video: " << source << endl;
		return -1;
	}

	string::size_type pAt = source.find_last_of('.');                  // Find extension point
	const string NAME = source.substr(0, pAt) + "_cropped.avi";   // Form the new name with container
	int ex = static_cast<int>(inputVideo.get(CAP_PROP_FOURCC));     // Get Codec Type- Int form

	// Transform from int to char via Bitwise operators
	char EXT[] = { (char)(ex & 0XFF), (char)((ex & 0XFF00) >> 8), (char)((ex & 0XFF0000) >> 16), (char)((ex & 0XFF000000) >> 24), 0 };
	Size S = Size((int)inputVideo.get(CAP_PROP_FRAME_WIDTH),    // Acquire input size
		(int)inputVideo.get(CAP_PROP_FRAME_HEIGHT));

	VideoWriter outputVideo;

	cout << "Input frame resolution: Width=" << S.width << "  Height=" << S.height
		<< " of nr#: " << inputVideo.get(CAP_PROP_FRAME_COUNT) << endl;
	cout << "Input codec type: " << EXT << endl;

	Rect markerRect;
	bool detection = false;
	bool startRecord = false;
	bool markerSaved = false;

	for (;;)
	{
		Mat src, gray;

		inputVideo >> src;              // read
		if (src.empty()) break;         // check if at end

		cvtColor(src, gray, COLOR_BGR2GRAY);

		if (mean(gray)[0] < MARKER_THRESHOLD) {
			if (!markerSaved) {
				markerRect = boundingRect(gray);
				cout << "Detected marker" << endl;
				cout << "Start: " << markerRect.tl().x << ", " << markerRect.tl().y << endl;
				cout << "End: " << markerRect.br().x << ", " << markerRect.br().y << endl;
				markerSaved = true;

				outputVideo.open(NAME, 0, inputVideo.get(CAP_PROP_FPS), markerRect.size(), true);

				if (!outputVideo.isOpened())
				{
					cout << "NAME: " << NAME << endl;
					cout << "Could not open the output video for write: " << NAME << endl;
					return -1;
				}
			}

			if (!startRecord)
				detection = true;
			else
				detection = false;

			continue;
		}

		if (detection) {
			startRecord = true;
			outputVideo << Mat(src, markerRect);
		}
	}

	cout << "Finished writing" << endl;
	return 0;
}
