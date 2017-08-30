#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>   

using std::string;

//Author: Sam Wolfe

/*The website that this program originally scraped has been updated
meaning that this program no longer works (and is, in fact, no longer needed)
*/

/*####### VIDEO CLASS ###########*/
class video : public sf::Sprite {

public:
	video(string videoLength, string videoLink, string videoSize, int videoNum);
	string length;   // Length of the video
	string link;	 // link to the video
	string size;	 // size of the video in MB
	sf::Texture texture; //The texture used (Don't try to draw this)
	int num;		 // video Number
	int pageNumber;

	void setImage(string path);
	void setPageNumber(int pageNum);
	bool clicked(sf::RenderWindow &window);
};

video::video(string videoLength, string videoLink, string videoSize, int videoNum) {
	length = videoLength;
	link = videoLink;
	size = videoSize;
	num = videoNum;
}

void video::setImage(string path) {
	texture.loadFromFile(path);
	setTexture(texture);
}

void video::setPageNumber(int pageNum) {
	pageNumber = pageNum;
}

bool video::clicked(sf::RenderWindow &window) {
	/*Checks if the mouse has clicked within all 4 bounds of the object*/
	/*AKA, if the mouse "clicked" the object*/
	if (sf::Mouse::getPosition(window).x > getPosition().x &&
		sf::Mouse::getPosition(window).x < getPosition().x + getGlobalBounds().width &&
		sf::Mouse::getPosition(window).y > getPosition().y &&
		sf::Mouse::getPosition(window).y < getPosition().y + getGlobalBounds().height)
	{
		return true;
	}
	else return false;
}

/*####### VIDEO DESCRIPTION CLASS ###########*/

class description : public sf::RectangleShape {

public:
	description(sf::Font &descFont);
	sf::Text dur;
	sf::Text size;
	sf::Text num;
	
	void updateText(video curVideo);
};

description::description(sf::Font &descFont) {
	setFillColor(sf::Color(89, 89, 89));

	dur.setFont(descFont);
	size.setFont(descFont);

	dur.setCharacterSize(18);
	size.setCharacterSize(18);

	dur.setFillColor(sf::Color::White);
	size.setFillColor(sf::Color::White);

	num.setFont(descFont);
	num.setCharacterSize(22);
	num.setFillColor(sf::Color::White);
	
	setOutlineColor(sf::Color(110, 110, 110));
	setOutlineThickness(1);
}

void description::updateText(video curVideo) {

	int offsetFromTopOfDescBox = 8;
	int offsetFromSidesOfDescBox = 65;

	//The following code places the text description centered beneath each image.
	//'dur' ,'size', and 'num' refer to the actual text objects being drawn to the screen
	dur.setPosition(curVideo.getPosition());
	dur.move(sf::Vector2f(offsetFromSidesOfDescBox, curVideo.getGlobalBounds().height + offsetFromTopOfDescBox));
	dur.setString(curVideo.length);
	dur.setOutlineThickness(1);
	dur.setOutlineColor(sf::Color::Black);

	size.setString(curVideo.size);
	size.setOutlineThickness(1);
	size.setOutlineColor(sf::Color::Black);

	int sizePositionX = curVideo.getPosition().x;
	sizePositionX = sizePositionX + curVideo.getGlobalBounds().width;
	sizePositionX = sizePositionX - (size.getGlobalBounds().width + offsetFromSidesOfDescBox);
	
	int sizePositionY = curVideo.getPosition().y;
	sizePositionY = sizePositionY + curVideo.getGlobalBounds().height + offsetFromTopOfDescBox;

	size.setPosition(sf::Vector2f(sizePositionX, sizePositionY));

	num.setPosition(curVideo.getPosition());
	num.move(sf::Vector2f(0, curVideo.getGlobalBounds().height + dur.getGlobalBounds().height + 4));
	num.setString(std::to_string(curVideo.num));
	num.setOutlineThickness(1);
	num.setOutlineColor(sf::Color::Black);

	int numPosX = curVideo.getPosition().x;
	numPosX = numPosX + curVideo.getGlobalBounds().width / 2;
	numPosX = numPosX - (num.getGlobalBounds().width / 2);

	int numPosY = curVideo.getPosition().y;
	numPosY = numPosY + curVideo.getGlobalBounds().height + 5;

	num.setPosition(numPosX, numPosY);

	setSize(sf::Vector2f(curVideo.getGlobalBounds().width, 40));
	setPosition(curVideo.getPosition());
	move(sf::Vector2f(0, curVideo.getGlobalBounds().height + 3));
}

/***********PROTOTYPES**********************/

void populateVideoList(std::vector<video> &videoList, std::vector<string> &rawAttributeLines);
void setImagePositions(std::vector<video> &videoList, int width, int height);
int getRawDur(string inputDur);
void sortByDuration(std::vector<video> &videoList);
std::vector<video> sortBySizeDurationRatio(std::vector<video> videoList);
void filterByDuration(std::vector<video> &videoList, std::vector<description> &descriptionList, int windowWidth, int windowHeight, sf::Font &myFont, string projectRoot);
void filterByRatio(std::vector<video> &videoList, std::vector<description> &descriptionList, int windowWidth, int windowHeight, sf::Font &myFont, string projectRoot);
void openInBrowser(string url);

/*******************MAIN********************/

int main() {

	sf::Font myFont;
	myFont.loadFromFile("OpenSans-Regular.ttf");

	string projectRoot = "-Location of the downloadPics.py python source file";

	std::string command = "python " + projectRoot + "/downloadPics.py";
	system(command.c_str()); //Yes this is very bad!!!! But this program wont be run by anybody other than myself. 

	std::vector<video> videoList;
	std::vector<sf::Texture> textureList; //The texture list is necessary to save the reference to the texture, otherwise images do not load
	std::vector<description> descriptionList;
	std::vector<string> rawAttributeLines;
	std::ifstream attributeFile;
	attributeFile.open(projectRoot + "/images/attributes.txt");


	//Process the file
	/*This text file was generated by the python script that was called above
	It contains all necessary information pulled directly from the website
	we are just parsing that text file here.*/
	string line;
	if (attributeFile.is_open())
	{
		while (getline(attributeFile, line))
		{
			//Push back the line to a vector
			rawAttributeLines.push_back(line);
		}
		attributeFile.close();
	}

	//Fill the videoList with parsed attributes
	populateVideoList(videoList, rawAttributeLines);

	for (int i = 0; i < videoList.size(); i++) {
		string path = (projectRoot + "/images/image");
		path += std::to_string(i);
		path += ".jpg";
		videoList[i].setImage(path);
	}

	//Selection of starting window size
	std::cout << std::endl << std::endl << "(1) Big Mode (1500x1200)" << std::endl << "(2) Small Mode (720x700) -DEFAULT-" << std::endl;
	string input = "";
	std::cin >> input;
	int windowWidth = 720;
	int windowHeight = 700;
	if (input == "1") {
		windowWidth = 1500;
		windowHeight = 1200;
	}

	//Give each video a page number and location
	setImagePositions(videoList, windowWidth, windowHeight);

	for (int i = 0; i < videoList.size(); i++) {
		descriptionList.push_back(description(myFont));
		descriptionList[i].updateText(videoList[i]);
	}

	std::cout << std::endl << std::endl << "1 - Filter By Length" << std::endl;
	std::cout << "2 - Filter by Size/Length ratio" << std::endl;

	int activePage = 0; //This variable determines the page that is drawn

	//#### GAME LOOP #####
	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Video Viewer");

	while (window.isOpen())
	{
		//########## EVENTS #################
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Right) {
					activePage++;
				}
				else if (event.key.code == sf::Keyboard::Left) {
					if (activePage != 0) {
						activePage--;
					}
				} //These lines adjust the window sized based on hotkey presses
				else if (event.key.code == sf::Keyboard::Num1) {
					filterByDuration(videoList, descriptionList, windowWidth, windowHeight, myFont, projectRoot);
				}
				else if (event.key.code == sf::Keyboard::Num2) {
					filterByRatio(videoList, descriptionList, windowWidth, windowHeight, myFont, projectRoot);
				}
			}
			if (event.type == sf::Event::MouseButtonPressed) {
				for (int i = 0; i < videoList.size(); i++) {
					if (videoList[i].pageNumber == activePage) {
						if (videoList[i].clicked(window)) {
							openInBrowser(videoList[i].link);
						}
					}
				}
			}
			if (event.type == sf::Event::MouseWheelMoved) {
				if (event.mouseWheel.delta > 0) {
					activePage++;
				}
				else {
					if (activePage != 0) {
						activePage--;
					}
				}
			}
		}

		//########### RENDERING ##################
		window.clear();

		//Draw all the images selected to be on the current page
		//setImagePositions should be called every time the window is resized or sorted
		for (int i = 0; i < videoList.size(); i++) {
			if (videoList[i].pageNumber == activePage) {
				window.draw(videoList[i]);
				window.draw(descriptionList[i]);
				window.draw(descriptionList[i].dur);
				window.draw(descriptionList[i].size);
				window.draw(descriptionList[i].num);
			}
		}

		window.display();
	}
	return 0;
}

void populateVideoList(std::vector<video> &videoList, std::vector<string> &rawAttributeLines) {

	int currentIndex = 0;

	int startIndex = 0;
	int endIndex = 0;

	string vidLength = "";
	string vidSize = "";
	string vidLink = "";
	int vidNum = 0;

	//This loop simply parses the text file line by line
	for (int i = 0; i < rawAttributeLines.size(); i++) {

		endIndex = rawAttributeLines[i].find(',', startIndex);
		vidSize = rawAttributeLines[i].substr(startIndex, endIndex);

		startIndex = endIndex + 1;

		endIndex = rawAttributeLines[i].find(',', startIndex);
		endIndex = endIndex - startIndex;
		vidLength = rawAttributeLines[i].substr(startIndex, endIndex);
		endIndex = endIndex + startIndex;
		startIndex = endIndex + 1;

		endIndex = rawAttributeLines[i].find(',', startIndex);
		endIndex = endIndex - startIndex;
		vidLink = rawAttributeLines[i].substr(startIndex, endIndex);
		endIndex = endIndex + startIndex;

		startIndex = endIndex + 1;
		vidNum = atoi(rawAttributeLines[i].substr(startIndex).c_str());

		startIndex = 0;
		endIndex = 0;

		videoList.push_back(video(vidLength, vidLink, vidSize, vidNum));
	}

}

void setImagePositions(std::vector<video> &videoList, int width, int height) {

	int imageWidth = videoList[0].getGlobalBounds().width;
	int imageHeight = videoList[0].getGlobalBounds().height; //Height and width of an individual image

	int verticalOffset = 60;
	int horizontalOffset = 10; //Distance between the images

	int xPosition = imageWidth + horizontalOffset;
	int yPosition = imageHeight + verticalOffset; //Actual adjusted width and height accounting for offset

	int imagesPerRow = width / xPosition;
	int imagesPerColumn = height / yPosition;
	int imagesPerPage = imagesPerRow * imagesPerColumn;	
	int numOfPages = ceil(videoList.size() / imagesPerPage); //Calculations used in the loops
	
	int center = (width - xPosition * imagesPerRow) / 2; //Used to center the images horizontally on the page

	for (int j = 0; j <= numOfPages; j++) {
		for (int i = 0; i < imagesPerColumn; i++) {
			for (int k = 0; k < imagesPerRow; k++) {
				
				if (videoList.size() > (j * imagesPerRow * imagesPerColumn) + (i * imagesPerRow) + k) {
					videoList[(j * imagesPerRow * imagesPerColumn) + (i * imagesPerRow) + k].setPosition(sf::Vector2f((xPosition * k)+center, yPosition * i));
					videoList[(j * imagesPerRow * imagesPerColumn) + (i * imagesPerRow) + k].setPageNumber(j);		
				}
			}
		}
	}
}

bool getLarger(video &one, video &two) {
	return (getRawDur(one.length) > getRawDur(two.length));
}

void sortByDuration(std::vector<video> &videoList) {
	std::cout << "Filtering..." << std::endl;
	std::sort(videoList.begin(), videoList.end(), getLarger);
	std::cout << "Done Filtering!" << std::endl;
}

std::vector<video> sortBySizeDurationRatio(std::vector<video> videoList) {
	std::vector<video> videoListSorted;

	return videoListSorted;
}

int getRawDur(string inputDur) {
	//Grab each set of times in the format XX:YY:ZZ
	int hours = atoi(inputDur.substr(0, 2).c_str());
	int minutes = atoi(inputDur.substr(3, 2).c_str());
	int seconds = atoi(inputDur.substr(6, 2).c_str());

	int total = hours * 3600 + minutes * 60 + seconds;

	return total;
}

//filterByDuration, believe it or not, filters by duration.
//Updates original video list.
void filterByDuration(std::vector<video> &videoList, std::vector<description> &descriptionList, int windowWidth, int windowHeight, sf::Font &myFont, string projectRoot) {
	sortByDuration(videoList);
	string text = "";
	descriptionList.clear();
	setImagePositions(videoList, windowWidth, windowHeight);
	for (int i = 0; i < videoList.size(); i++) {
		descriptionList.push_back(description(myFont));
		descriptionList[i].updateText(videoList[i]);
		text = descriptionList[i].dur.getString();
	}
	for (int i = 0; i < videoList.size(); i++) {
		string path = (projectRoot + "/images/image");
		path += std::to_string(videoList[i].num - 1);
		path += ".jpg";
		videoList[i].setImage(path);
	}
}

//Return the size:length ratio of the video
float getRatio(video inputVideo) {

	int rawDur = getRawDur(inputVideo.length);
	float rawDurAsFloat = rawDur;
	float rawSize = atof(inputVideo.size.c_str());

	return rawDur / rawSize;
}

bool getLargerRatio(video &one, video&two) {
	return(getRatio(one) > getRatio(two));
}

void sortByRatio(std::vector<video> &videoList) {
	std::cout << "Filtering By Ratio..." << std::endl;
	std::sort(videoList.begin(), videoList.end(), getLargerRatio);
	std::cout << "Done Filtering!" << std::endl;
}

void filterByRatio(std::vector<video> &videoList, std::vector<description> &descriptionList, int windowWidth, int windowHeight, sf::Font &myFont, string projectRoot) {
	sortByRatio(videoList);
	string text = "";
	descriptionList.clear();
	setImagePositions(videoList, windowWidth, windowHeight);
	for (int i = 0; i < videoList.size(); i++) {
		descriptionList.push_back(description(myFont));
		descriptionList[i].updateText(videoList[i]);
		text = descriptionList[i].dur.getString();
	}
	for (int i = 0; i < videoList.size(); i++) {
		string path = (projectRoot + "/images/image");
		path += std::to_string(videoList[i].num - 1);
		path += ".jpg";
		videoList[i].setImage(path);
	}
}

//opens the current URL in the browser (this opens directly to the page of the selected video)
void openInBrowser(string url) {
	string command = "chrome.exe " + url;
	system(command.c_str()); //BAD!!
}