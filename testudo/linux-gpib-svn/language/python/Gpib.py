
import gpib

RQS = (1<<11)
SRQ = (1<<12)
TIMO = (1<<14)


class Gpib:
	'''Three ways to create a Gpib object:
	Gpib("name")
		returns a board or device object, from a name in the config file
	Gpib(board_index)
		returns a board object, with the given board number
	Gpib(board_index, pad[, sad[, timeout[, send_eoi[, eos_mode]]]])
		returns a device object, like ibdev()'''
	
	def __init__(self, name = 'gpib0', pad = None, sad = 0, timeout = 13, send_eoi = 1, eos_mode = 0):
		self._own = False
		if isinstance(name, basestring):
			self.id = gpib.find(name)
			self._own = True
		elif pad is None:
			self.id = name
		else:
			self.id = gpib.dev(name, pad, sad, timeout, send_eoi, eos_mode)
			self._own = True
	
	# automatically close descriptor when instance is deleted
	def __del__(self):
		if self._own:
			gpib.close(self.id)
			
	def __repr__(self):
		return "%s(%d)" % (self.__class__.__name__, self.id)


	def command(self,str):
		gpib.command(self.id, str)
	
	def config(self,option,value):
		self.res = gpib.config(self.id,option,value)
		return self.res
	
	def interface_clear(self):
		gpib.interface_clear(self.id)
	
	def write(self,str):
		gpib.write(self.id, str)

	def write_async(self,str):
		gpib.write_async(self.id, str)
	
	def read(self,len=512):
		self.res = gpib.read(self.id,len)
		return self.res

	def listener(self,pad,sad=0):
		self.res = gpib.listener(self.id,pad,sad)
		return self.res

	def ask(self,option):
		self.res = gpib.ask(self.id,option)
		return self.res

	def clear(self):
		gpib.clear(self.id)
		
	def wait(self,mask):
		gpib.wait(self.id,mask)
	
	def serial_poll(self):
		self.spb = gpib.serial_poll(self.id)
		return self.spb

	def trigger(self):
		gpib.trigger(self.id)

	def remote_enable(self,val):
		gpib.remote_enable(self.id,val)

	def ibsta(self):
		self.res = gpib.ibsta()
		return self.res

	def ibcnt(self):
		self.res = gpib.ibcnt()
		return self.res

	def timeout(self,value):
		return gpib.timeout(self.id,value)
