class Line:
	def __init__(self,x0,y0,x1,y1,style):
		self.x0,self.y0,self.x1,self.y1,self.style=x0,y0,x1,y1,style
	
class Sprite:	
	def __init__(self):
		self.x=0
		self.y=0
		self.rotation=0
		self.scale=1
		self.curx=0
		self.cury=0
		self.objects=[]
		self.linestyle={"color":(0,0,0), "width":1}
	
	def add(self,obj):
		self.objects.append(obj)
	
	def moveTo(self,x,y):
		self.curx,self.cury=x,y
	
	def lineTo(self,x,y):
		self.objects.append(Line(self.curx,self.cury,x,y,self.linestyle.copy()))
		self.curx,self.cury=x,y
	
	def lineStyle(self,color=None,width=-1):
		if color: self.linestyle["color"]=color
		if width>=0: self.linestyle["width"]=width
