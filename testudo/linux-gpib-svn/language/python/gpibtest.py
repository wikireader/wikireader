from time import sleep
from Gpib import *

v = Gpib('voltmeter')

v.clear()

v.write('D0 Q0')

sleep(1)

for i in range(0,10):
	print v.read()



