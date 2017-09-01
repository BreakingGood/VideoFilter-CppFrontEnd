# VideoFilter-CppFrontEnd
An updated version of my old video sorting program.

This program is designed to scrape a particular website using the BeautifulSoup Python library https://www.crummy.com/software/BeautifulSoup/. It then parses the HTML in such a way that particular attributes regarding this lectuerer's videos can be more easily sorted and filtered.

Sorting options include filtering by video length, video file size, and the video size:length ratio.

After the parsing is complete, the videos are displayed in a window using the C++ SFML library https://www.sfml-dev.org/. This library is used to handle all mouse/keyboard inputs, reading and displaying .png image files, and drawing the window itself.

