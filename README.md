# Gerrymander
A turn-based logic game in C++, using Qt libraries


Compiling:

You'll need to download and install a copy of Qt Creator 3.6.1 or later, with the Qt 5.6 libraries.

Load the project in Qt Creator via the gerrymander.pro file; It should be compatible with Windows, Linux, or any other platform that Qt Creator can use. Run qmake and build. (The main window is hard-coded to 1920x1080 resolution, though, so it's probably not suitable for small Android devices.)

Note: If you haven't done it already, you'll need to set the location of the Qt libraries in your machine's path variable.


Rules:

This is the single-screen version of a game I call "Gerrymander" (inspired by a small game called "Patchwork," by Uwe Rosenberg). In this game, you and your opponent are politicians trying to raise as much money and get as many votes as possible from your territory.

Territory is broken up into gerrymandered districts that appear as shapes around the edge of the screen. Some districts have coin shapes, indicating the number of high-rolling contributors in that area. As you use your campaign funds to buy up available districts and place them on your tally board, you will acquire contributors as well.

Each district has a certain listed cost and time. After you buy up the district, your marker on the calendar will be advanced the amount of time for that district. Whenever you pass a coin shape on the calendar, major contributors from the districts you've secured will kick in another round of funding for your campaign, allowing you to buy up more districts.

You can only grab the district shapes highlighted around the edge of the board if you have enough money. If you're short on campaign funds, you can select a date further along on the calendar and collect the ordinary funds that trickle in by that date.

You and your opponent take turns either buying up districts or advancing along the calendar. Your turn lasts as long as your token isn't out ahead of your opponent's token. In other words, if you buy a district and you're still behind your opponent on the calendar, you can make another move. Even if you land on top of your opponent, you can still make another move -- your turn ends only when you're ahead.

The calendar also shows about five single-sqaure townships that can be used to fill in gaps on your tally board. The first politician to land on or beyond one of these township squares on the calendar will receive it. (Note that even if your turn is over, if there is one or more township squares available to you, your turn will be prolonged until your take them.)

You want to pack your tally board as tightly as possible, because final scoring will be the total of the money you've raised minus the number of empty squares on your tally board. The game isn't over until both players enter the darkened area in the center of the calendar.
