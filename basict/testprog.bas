5 gosub 300
7 print
10 print "Blinky auf GPIOA7"
20 print "GPIOB1 bestimmt Frequenz"
30 print "high = schnell  low = langsam"
40 f=12500
50 if in(8)= 1 f=5000
60 out(7)=0
70 gosub 200
80 out(7)=1
90 gosub 200
100 goto 40
200 rem Delay
210 for i= 1 to f
220 a=a+1
230 next i
240 return
300 print "Die Ascii Tabelle"
302 print
305 b= 0
310 for a= 32 to 127
315 if a> 99 goto 320
317 scall 1,32
320 print a,": ",
325 scall 1,a
330 print "  ",
340 b= b+1
350 if b< 10 goto 380
360 scall 1,10
370 scall 1,13
375 b= 0
380 next a
390 print
400 return
