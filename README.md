# Nano clone, 32 mhz
Some examples for an inexpensive double speed nano clone

I recently saw a [youtube video by Ralph Bacon](https://www.youtube.com/watch?v=Myfeqrl3QP0) about a new clone of the arduino nano that runs double speed and is dirt cheap.
The board described isn't an exact clone, and requires another board support library.  
[David Buezas](https://github.com/dbuezas) provides an excellent one here on github.
The board in question is available at [Banggod](https://www.banggood.com/3pcs-Wemos-TTGO-XI-8F328P-U-Board-Motherboard-For-Arduino-Nano-V3_0-Promini-Or-Replace-p-1338041.html?rmmds=detail-left-hotproducts__1&cur_warehouse=CN) among other places 
for well under 10 dollars for 3, so it seemed a low to no risk thing to give a whirl.  It appears to work as advertised.

I ported what is a standard application here to it, making a few changes to take advantage of the better speed, and
higher A/D resolution.  This sketch does some basic data acq and spits it out over the serial link at 115200 baud.  
It samples the first 4 A/D channels and a hardware counter at 10hz, and outputs this data along with the time 
since reset in ms in plain ASCII.  The format is time AD0 AD1 AD2 AD3 counts \n using tabs for the whitespace.

I originally did this work on a Raspberry Pi 4, using arduino IDE 1.8.10, but have since tested it on a couple of x86
NUC machines, where everything is of course, faster, and the high baud rate is better handled.

Here's the board support string to add in your arduino preferences:

    https://raw.githubusercontent.com/dbuezas/lgt8fx/master/package_lgt8fx_index.json
    
Info in the file LGinfo.txt, and of course, in the sketch itself.
Not only does the A/D have more bits of resolution than a stock Nano, in this sketch, 8 samples per interval
are taken and summed.  The raw result is reported, and you'll just have to take that into account in any scaling.
This is done to let any noise dither the result a little and get any extra goodness that might be there.
It's an old DSP trick...

Bonus is the toggle sketch, which simply shows how to use the new fastio feature to toggle a pin.  If you hook up
a scope to that pin, the speed at which this thing gets around the loop() function is pretty amazing.

I also did this in the data acq sketch, so I could see what effect other things had on the timing.
