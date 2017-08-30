import sys, urllib.request
import os
import webbrowser
import requests
import math
import time
import shutil
from bs4 import BeautifulSoup

#THIS FIXES ENCODING ERRORS
#chcp 65001

#Script called by main.cpp to scrape and parse the HTML

#Gets URLs for all of the images on the page
def getPageData(soup, imageList, attrList, linkList, pageNumList, pageNum):
    #Locate all img tags and get their src (This is the image direct link)
    #Then add it to list
    for image in soup.find_all('img'):
        text = image.get('src')
        if(".jpg" in text):
            imageList.append(text)
            pageNumList.append(pageNum)

    #Locate all the a tags and get their href attribute (This will give all links on the page)
    #They need to be filtered out a bit       
    for image in soup.find_all('a'):
        text = image.get('href')
        if(".bz" not in text) and ("dmca-form" not in text) and ("javascript" not in text) and (len(text) > 10):
            linkList.append(text)
    
    #Locate all paragraph tags, get all of the text, and add it to a list
    #This will effectively include all of the text under an image
    for page in soup.find_all('p'):
        if(len(page) > 1):
            attrList.append(page.getText())

def getTotalPages(pageHTML):
    #Do a lot of shit to get the total num of pages
    fullPageText = pageHTML
    firstNumIndex = (fullPageText.find("Page ") + 5)
    if(firstNumIndex != 4):
        while(fullPageText[firstNumIndex].isnumeric()):
            firstNumIndex = firstNumIndex + 1
        
        firstNumIndex = firstNumIndex + 4
        allNumChars = ""
        while(fullPageText[firstNumIndex].isnumeric() or fullPageText[firstNumIndex] == ","):
            if(fullPageText[firstNumIndex] != ","):
                allNumChars = allNumChars + fullPageText[firstNumIndex]
            firstNumIndex = firstNumIndex + 1
    else:
        allNumChars = ("1")

    return int(allNumChars)

#Break down the attrList into manageable parts
def parseAttrList(attrList, sizeList, durList):
    for idx, attr in enumerate(attrList): 
        curAttrString = attrList[idx]

        #Get the size
        sizeIndexStart = curAttrString.index("(")
        sizeIndexEnd = curAttrString.index(")")
        sizeSubstring = curAttrString[sizeIndexStart+1:sizeIndexEnd]
        sizeIndexStart = sizeSubstring.index(" ")
        sizeSubstring = sizeSubstring[:sizeIndexStart]
        sizeList.append(sizeSubstring)

        #Get the duration
        durIndexStart = curAttrString.index("duration")
        durIndexStart = durIndexStart + 10
        durIndexEnd = durIndexStart + 8
        durSubstring = curAttrString[durIndexStart:durIndexEnd]
        durList.append(durSubstring)

    del attrList[:]

#Fill the videoList with video objects containing all necessary attributes
def populateVideoList(videoList, imageListURLs, sizeList, durList, linkList, pageNumList):
    for idx, image in enumerate(imageListURLs):
        videoList.append(video(image,sizeList[idx],durList[idx], linkList[idx], pageNumList[idx]))
    del imageListURLs[:]
    del sizeList[:]
    del durList[:]
    del linkList[:]

def generatePageLinks(totalPages, pageLinkURLs, name):
    for idx in range(2,totalPages+1):
        link = "http://website not public.org/page/" + str(idx) + "/?s=" + name
        pageLinkURLs.append(link)

class video:
    def __init__(self, imageURL, size, dur, link, pageNum):
        self.image = imageURL
        self.size = size
        self.dur = dur
        self.link = link
        self.page = str(pageNum)

########### START GAME INITIALIZATION ##################################################################################
profName = input("Input Professor Name \n")
downloaded = input("Pictures already downloaded? (y/n): ")

landingPage = "http://website not public.org/?s=" + profName
pageRequest = requests.get(landingPage)
pageHTML = pageRequest.text
soup = BeautifulSoup(pageHTML, 'html.parser')

pageLinkURLs = []   #List of links to each page

totalPages = getTotalPages(pageHTML) #Total number of pages in the given search

choice = input(str(totalPages) + " Pages found. How many should be searched (0 for all): ")
choice = int(choice)
if(choice != 0):
    totalPages = choice

imageListURLs = []  #List of the IMAGE URLs
attrList = []       #List of the unformatted lines of attribute text
linkList = []       #List of all of the actual page links for videos
sizeList = []       #List of the video sizes (Format: XX.XX as a string)
durList = []        #List of the video durations (Format: XX:XX:XX as a string)
videoList = []      #Formatted container of Image URLs, (Format: Siz)
pageNumList = []

#Fill a list with links to all pages, excludes the landing page
generatePageLinks(totalPages, pageLinkURLs, profName)

#These variables are used to monitor whether or not inaccurate data was received
#If they are not all equal at the end of the coming loop, something went wrong
totalCountAttr = 0
totalCountSize = 0
totalCountDur = 0
totalCountVideo = 0
totalCountLink = 0
totalCountList = 0

currentPageNum = 1

#Parse the landing page first
getPageData(soup, imageListURLs, attrList, linkList, pageNumList, currentPageNum) #0 because this is the index page
totalCountAttr += len(attrList)
totalCountLink += len(linkList)
totalCountList += len(imageListURLs)

#Get the attributes of the landing page
parseAttrList(attrList, sizeList, durList)
totalCountSize += len(sizeList)
totalCountDur += len(durList)
print("PAGE 1 PARSED")
#Build first set of video objects
populateVideoList(videoList, imageListURLs, sizeList, durList, linkList, pageNumList)

completePages = 0
currentPage = ""
while(completePages < totalPages - 1):
    currentPageNum = currentPageNum + 1
    print("PAGE " + str(currentPageNum) + " PARSED")
    currentPage = pageLinkURLs[completePages]
    pageRequest = requests.get(currentPage)
    pageHTML = pageRequest.text
    soup = BeautifulSoup(pageHTML, 'html.parser')

    #Generate lists of images and attributes using a given HTML container (soup)
    getPageData(soup, imageListURLs, attrList, linkList, pageNumList, currentPageNum)
    #Get the counts of these lists for comparison later
    totalCountAttr += len(attrList)
    totalCountLink += len(linkList)
    totalCountList += len(imageListURLs)

    #Turn the raw attrList data into sizeList and durList lists
    parseAttrList(attrList, sizeList, durList)
    #Get the counts of these lists for comparison later
    totalCountSize += len(sizeList)
    totalCountDur += len(durList)

    #Fill the videoList with info from several containers
    populateVideoList(videoList, imageListURLs, sizeList, durList, linkList, pageNumList)
    completePages = completePages + 1

#-------------------------------------------------------------------------------------#
totalCountVideo = len(videoList)

if(totalCountSize != totalCountVideo):
    print("WARNING, VIDEO SIZES POTENTIALLY INCORRECT" + str(totalCountSize) +" vs " + str(totalCountVideo))
if(totalCountDur != totalCountVideo):
    print("WARNING, VIDEO DURATIONS POTENTIALLY INCORRECT")
if(totalCountAttr != totalCountVideo):
    print("WARNING, ATTRIBUTES POTENTIALLY INCORRECT")
if(totalCountLink != totalCountVideo):
    print("WARNING, LINKS POTENTIALLY INCORRECT")
if(totalCountList != totalCountVideo):
    print("WARNING, IMAGES POTENTIALLY INCORRECT")

totalImagesCollected = totalCountList

if(downloaded != "y"):

    #The following code clears the directory beforehand, getting rid of leftover images
    folder = 'image directory'
    for the_file in os.listdir(folder):
        file_path = os.path.join(folder, the_file)
        try:
            if os.path.isfile(file_path):
                os.unlink(file_path)
            #elif os.path.isdir(file_path): shutil.rmtree(file_path)
        except Exception as e:
            print(e)

    for videoNum, video in enumerate(videoList):
        path = "image directory" + str(videoNum) +".jpg"
        urllib.request.urlretrieve(video.image,path)
        print(str(videoNum) + "/" + str(totalImagesCollected - 1))

fileOutput = open("directory containing attributes.txt",'w')

videoNumber = 1
for video in videoList:
    formattedLine = str(video.size) + "," + str(video.dur) + "," + str(video.link) + "," + str(videoNumber) + "\n"
    fileOutput.write(formattedLine)
    videoNumber = videoNumber + 1
print("Done Writing...")
fileOutput.close()