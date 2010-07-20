/********************************************************
This is the sourcecode of PhotoSphere by Kevin Allekotte
To compile it install HaXe and run:
haxe -swf9 photosphere.swf -main Sphere -D network-sandbox -swf-header 400:400:30:99aacc

The following classes are defined:

Sphere:
	This is the main class. It loads the specified xml
	with the description of the thumbnails (either from
	picasa or a custom xml) and displays the sphere.
	The most important functions are:
	main:
		Initializes everything. It reads the flashvars
		parameters and creates the URLLoader to load the
		xml.
	xmlloaded:
		This function is called as soon as the xml file
		finishes loading. It reads the thumbnails and link
		urls from the file and creates a Thumb instance for
		each one. It populates the photos sprite with all
		the thumbnails and gives the starting position for
		each one.
	frame:
		This is called at every frame (about 30 times per
		second). It recalculates the position of the
		thumbnails according to mouse movement and renders
		the scene.
	sortphotos:
		This is called periodically (when necesary) by frame.
		It corrects the z-ordering of the Thumbs according to
		their z value.

Thumb:
	This class is an extension of Sprite and
	represents a thumbnail on the sphere. It has a 3D posi-
	tion and contains the image of the thumbnail with border
	and shadow.
	The main functions are:
	new:
		The constructor, creates an instance. It takes an url
		of the thumbnail image file and optionally an url wich
		opens on click. it loads the image and sets its proper-
		ties.
	loaded:
		This function is called when the thumbnail finishes loading.
		It resizes the image and adds the border.
	display:
		This is the function that sets its position on the screen
		and scaling and alpha of the sprite according to its current
		3D position and the size of the sphere and number of thumbnails.
	scalingFunction:
		This function decides the scale of the thumbnail given its 3D
		position.

V3:
	This class represents Vectors in 3D space and some operations on them.
	it implements normalize, add, subtract, cross product, dot product, 
	scalar scaling, etc.

Loader:
	This class represents a simple loading animation. It consists of N dots
	wich change color every m milliseconds. You can set the time interval,
	the colors, the quantity, the separation and the size.
********************************************************/

import flash.display.Sprite;

class PhotoSphere{
	static var params:Dynamic<String>;
	/*static var service:String;
	static var user:String;
	static var album:String;
	static var thumbquality:String;
	static var xml:String;
	static var max:String;*/
	static function main(){
		params = flash.Lib.current.loaderInfo.parameters; //read the flashvars parameters
		flash.Lib.current.addChild(new Sphere(params.service, params.user, params.album, Std.parseInt(params.thumbnailquality), params.xml, Std.parseInt(params.max)));
	}
}

class Sphere extends Sprite{
	// Initial Rotational Speed: the speed the sphere rotates before the mouse enters for the first time;
	private var initialRotationSpeed:Float;
	private var rotationSpeed:Float;
	private var service:String;
	private var user:String;
	private var album:String;
	private var loadin:Loader;
	private var thumbs:Array<Thumb>;
	private var framenum:Int;
	private var n:Int;
	private var photos:Sprite;
	private var r:Float;
	private var phi:Float;
	private var theta:Float;
	private var mouseonstage:Bool;
	private var waiting:Bool;
	private var _width:Int;
	private var _height:Int;
	private var thumbquality:Int;
	private var max:Int;
	private var orient:V3;
	private var goto:V3;
	private var dbg:Sprite;
	
	public function new(?_service:String="picasa", ?_user:String="kevinalle", ?_album:String="StarredPhotos", ?_thumbquality:Int=5, ?_xml:String=null, ?_max:Int=0){
		super();
		service=_service!=null?_service:"picasa";
		user=_user!=null?_user:"kevinalle";
		album=_album==null&&user=="kevinalle"?"StarredPhotos":_album;
		thumbquality=_thumbquality;
		max=_max;
		initialRotationSpeed=0.003;
		rotationSpeed=0.15;
		r=100;
		phi=0.003;
		theta=0;
		mouseonstage=false;
		waiting=true;
		_width=400;
		_height=400;
		loadin=new Loader();
		thumbs=new Array(); //This array will hold all the thumbnails
		framenum=0; //a frame counter
		photos=new Sprite(); //This sprite will contain all thumbnails
		
		var xmlLoader:flash.net.URLLoader=null;
		if(service=="picasa"){
			//We have to set all the security stuff to be able to load things from picasa web service
			//it is necesary to make cross domain requests
			flash.system.Security.allowDomain("http://picasaweb.google.com");
			flash.system.Security.allowInsecureDomain("http://picasaweb.google.com");
			flash.system.Security.loadPolicyFile("http://photos.googleapis.com/data/crossdomain.xml");
			var thumbsizes=[32, 48, 64, 72, 104, 144, 150, 160]; //these are thethumbnail sizes picasa supports
			var thsz:Int=thumbquality>0&&thumbquality<=8?thumbquality:5; //we chose one of the sizes according to thumbquality
			//This is the url of the xml we will request
			var url="http://photos.googleapis.com/data/feed/api/user/"+user+(album!=null?"/album/"+album:"")+"?thumbsize="+thumbsizes[thsz-1]+"c";
			xmlLoader = new flash.net.URLLoader(new flash.net.URLRequest(url)); //Make the request
		}else if(service=="custom"){
			xmlLoader = new flash.net.URLLoader(new flash.net.URLRequest(_xml));
		}
		loadin.x=_width/2;loadin.y=_height/2;
		this.graphics.beginFill(0,0);
		this.graphics.drawRect(0,0,_width,_height);
		this.graphics.endFill();
		this.addChild(loadin);// display the loader animation
		xmlLoader.addEventListener(flash.events.Event.COMPLETE, xmlloaded); //when the xml loads, call xmlloaded
		this.addEventListener(flash.events.MouseEvent.ROLL_OUT, mouseout); //update mouseonstage whenever mouse leaves the sprite
		this.addEventListener(flash.events.MouseEvent.ROLL_OVER,mousemove); //update mousonstage when mouse is active and cancel the waiting animation
	}
	private function mouseout(e:flash.events.MouseEvent){ mouseonstage=false; }
	private function mousemove(e:flash.events.MouseEvent){ mouseonstage=true;waiting=false; }
	private function xmlloaded(evt:flash.events.Event){
		//trace("Loaded :P");
		this.removeChild(loadin);
		var xml:flash.xml.XML=new flash.xml.XML(evt.target.data); //Parse the xml
		var entrys:flash.xml.XMLList=null;
		//define some namespaces the picasa service uses
		var atom:flash.utils.Namespace =  new flash.utils.Namespace(xml.namespace());
		var gphoto:flash.utils.Namespace =  new flash.utils.Namespace("http://schemas.google.com/photos/2007");
		var media:flash.utils.Namespace =  new flash.utils.Namespace("http://search.yahoo.com/mrss/");
		//get the items of the photos
		if(service=="picasa"){
			entrys=xml.child( ns(atom,"entry") );
		}else if(service=="custom"){
			entrys=xml.child("photo");
		}
		n=entrys.length(); // n is the amount of thumbnails
		if(max>0){n=Math.round(Math.min(n,max));} //limit the amount of thumbnails if requested by parameter max
		r=Math.floor(Math.min(_width/2,_height/2)*.7);//Set the radius of the sphere
		
		for(i in 0...n){
			var thumb:String="";var url:String="";
			if(service=="picasa"){
				thumb=entrys[i].child(ns(media,"group"))[0].child(ns(media,"thumbnail"))[0].attribute("url").toString(); //read the url of the thumbnail
				url=entrys[i].child(ns(atom,"link"))[1].attribute("href").toString(); //get the url of the link
			}else if(service=="custom"){
				thumb=entrys[i].attribute("src").toString(); //get the thumbnail url
				url=entrys[i].attribute("href").toString(); //get the url of the link
			}
			thumbs.push(new Thumb(thumb,url)); //create a thumbnail and add it to our array of thumbnails
			photos.addChild(thumbs[i]); //add the Sprite to the scene
		}
		shuffle(thumbs); // Shuffle the thumbnails so they appear in random order
		photos.addEventListener(flash.events.Event.ENTER_FRAME,frame); //Call frame in every frame to update the scene
		//center the sphere
		photos.x=_width/2;
		photos.y=_height/2;
		this.addChild(photos); //display the sphere in pur scene
		
		//This part distributes the thumbnails eavenly around the sphere, refer to the link for more details
		//Spiral points on sphere: http://sitemason.vanderbilt.edu/page/hmbADS#spiral
		var s = 3.6/Math.sqrt(n);
		var long = 0.;
		var dz = 2./n;
		var z = 1-dz/2;
		for(k in 0...n){
			var rr = Math.sqrt(1-z*z);
			thumbs[k].pos = new V3(Math.cos(long)*rr*r, Math.sin(long)*rr*r, z*r);
			thumbs[k].z=z*r;
			z-=dz;
			long+=s/rr;
		}
		
		//update each thumbnails Sprite and z-order them
		for(i in 0...n) thumbs[i].display(thumbs[i].pos,n,r);
		sortphotos();
	}
	
	private function frame(evt:flash.events.Event){
		//This is called every frame. it must redraw the scene
		framenum++;
		if(mouseonstage){
			//we will rotate the sphere according to the mouse position
			theta=-photos.mouseY*rotationSpeed/1000.;
			phi=-photos.mouseX*rotationSpeed/1000.;
		}else if(!waiting){
			//if mouse is away apply some friction to stop rotation smoothly
			theta*=.9;
			phi*=.9;
		}
		//calculate sines and cosines of rotating angles
		var st=Math.sin(theta);
		var ct=Math.cos(theta);
		var sp=Math.sin(phi);
		var cp=Math.cos(phi);
		for(i in 0...n){
			//for each thumbnail update its position rotating it around (0,0,0) by phi and theta.
			//This is the same as multiplying the position by a rotation matrix
			var x=thumbs[i].pos.x;
			var y=ct*thumbs[i].pos.y+st*thumbs[i].pos.z;
			var z=-st*thumbs[i].pos.y+ct*thumbs[i].pos.z;
			var x2=cp*x+sp*z;
			var y2=y;
			var z2=-sp*x+cp*z;
			thumbs[i].pos=new V3(x2,y2,z2); //updat the position
			thumbs[i].display(new V3(x2,y2,z2),n,r); //ask thumbnail to redisplay itself
			thumbs[i].z=z2; //update its z value, for z-ordering
		}
		//if necessary, z-sort the thumbnails
		//Doing this every 10 frames is more CPU friendly
		if((phi!=0||theta!=0) && framenum%10==0) sortphotos();
	}
	
	private function ns(namespace:flash.utils.Namespace, name:String){
		//create a Qname with a namespace and a element name.
		return new flash.utils.QName(namespace,new flash.utils.QName(name));
	}
	
	private function sortphotos(){
		//sort the thumbnails array by its z value
		thumbs.sort(function(a,b:Thumb){return a.z==b.z?0:a.z>b.z?1:-1;});
		//update the thumbnails z-Index
		var i=n;
		while(--i>=0) photos.setChildIndex(thumbs[i], i);
	}
	
	private function shuffle<T>(arr:Array<T>){
		//a function to shuffle an array
		var n = arr.length;
		while (n > 1){
			var k = Std.random(n);
			n--;
			var temp = arr[n];
			arr[n] = arr[k];
			arr[k]= temp;
		}
	}
	
/*	private function axisrotate(as:V3,th:Float,xyz:V3){
		//this function rotates a vector around an axis
		var axis:V3=as.normalized();
		var p=new Quaternion(0, xyz.x, xyz.y, xyz.z);
		var c=Math.cos(th/2.);var s = Math.sin(th/2.);
		var rot=new Quaternion(c, axis.x*s, axis.y*s, axis.z*s);
		var res=rot.mult(p).mult(rot.conj());
		return new V3(res.i,res.j,res.k);
	}*/
}

class Thumb extends Sprite{
	//public var img:Sprite;
	public var link:String;
	public var pos:V3;
	public var z:Float;
	private var r:Float;
	
	public function new(url:String,?link:String){
		super();
		var ldr:flash.display.Loader = new flash.display.Loader();
		ldr.name="loader";
		ldr.load(new flash.net.URLRequest(url));
		ldr.contentLoaderInfo.addEventListener(flash.events.Event.COMPLETE,this.loaded);
		//this.img=new Sprite();
		this.rotation=(Math.random()-.5)*20;
		var drpShdw=new flash.filters.DropShadowFilter(2,45,0,1,8,8,.6,1);
		this.filters=[drpShdw];
		this.addChild(ldr);
		this.link=link;
		if(this.link!=null) this.addEventListener(flash.events.MouseEvent.CLICK,openurl);
		this.addEventListener(flash.events.MouseEvent.MOUSE_OVER,over);
	}
	
	private function loaded(?evt:flash.events.Event){
		var ldr=this.getChildByName("loader");
		var w=ldr.width;var h=ldr.height;
		if(w>h){ h=h*56/w;w=56; }
		else{ w=w*56/h;h=56; }
		ldr.width=w;ldr.height=h;
		var sze=ldr.width>ldr.height?ldr.width:ldr.height;
		this.buttonMode=true;
		this.graphics.beginFill(0xffffff);
		this.graphics.drawRect(-sze/2-4,-sze/2-4,sze+8,sze+8);
		this.mouseChildren=false;
		ldr.x=-ldr.width/2;
		ldr.y=-ldr.height/2;
	}
	
	public function display(pos:V3,n:Int,r:Float){
		this.z=pos.z;
		this.x=pos.x;
		this.y=pos.y;
		this.alpha=pos.z/(3.*r)+.66;
		this.scaleX=this.scaleY=scalingFunction(n,r,pos.z);
	}
	
	private function scalingFunction(n:Int,r:Float,z:Float){
		return (4.5/Math.sqrt(n))*(.8*z/(2.*r)+.7)*(4./3);
	}
	
	function over(evt:flash.events.MouseEvent){
		var rect=new Sprite();
		rect.graphics.beginFill(0,0);
		rect.graphics.drawRect(-1.2*this.width/2,-1.2*this.height/2,1.2*this.width,1.2*this.height);
		rect.name="rect";
		this.addChild(rect);
		//this.pos.scaleBy(1.1);
		this.r=this.pos.norm();
		this.removeEventListener(flash.events.Event.ENTER_FRAME,shrink);
		this.addEventListener(flash.events.Event.ENTER_FRAME,grow);
		this.addEventListener(flash.events.MouseEvent.MOUSE_OUT,out);
	}
	function out(evt:flash.events.MouseEvent){
		this.removeChild(this.getChildByName("rect"));
		this.removeEventListener(flash.events.MouseEvent.MOUSE_OUT,out);
		this.removeEventListener(flash.events.Event.ENTER_FRAME,grow);
		this.addEventListener(flash.events.Event.ENTER_FRAME,shrink);
		//this.pos.scaleBy(1/1.1);
		//this.pos.normalize(this.r);
	}
	function grow(e:flash.events.Event){
		if(this.pos.norm()<this.r*1.2) this.pos.scaleBy(1.04);
	}
	function shrink(e:flash.events.Event){
		if(this.pos.norm()>this.r) this.pos.scaleBy(.96);
		else this.removeEventListener(flash.events.Event.ENTER_FRAME,shrink);
	}
	
	function openurl(evt:flash.events.MouseEvent){
		var url = new flash.net.URLRequest(this.link);
		flash.Lib.getURL(url, "_blank");
	}
	
	public function normalize(r:Float){
		this.pos.normalize(r);
	}
}

class V3{
	//This class represents a Vector in 3D space and some operations on it.
	public var x:Float;
	public var y:Float;
	public var z:Float;
	public function new(x:Float,y:Float,z:Float){
		this.x=x;
		this.y=y;
		this.z=z;
	}
	public function normalize(?r:Float=1){
		//Scale the vector so its length is equal to R
		var norm=Math.sqrt(this.x*this.x+this.y*this.y+this.z*this.z);
		this.x*=r/norm;
		this.y*=r/norm;
		this.z*=r/norm;
	}
	public function normalized(?r:Float=1){
		//Return a new vector which has the same direction but length r
		var norm=Math.sqrt(this.x*this.x+this.y*this.y+this.z*this.z);
		return new V3(this.x*r/norm, this.y*r/norm, this.z*r/norm);
	}
	public function add(v:V3){
		//add this vector with another and return the result
		return new V3(this.x+v.x,this.y+v.y,this.z+v.z);
	}
	public function subtract(v:V3){
		//subtract another vector and return the result
		return new V3(this.x-v.x,this.y-v.y,this.z-v.z);
	}
	public function scaleBy(s:Float){
		//scalar product or scaling
		this.x*=s;
		this.y*=s;
		this.z*=s;
	}
	public function scaledBy(s:Float){
		//returnes a scaled copy of the vector
		return new V3(this.x*s,this.y*s,this.z*s);
	}
	public function norm(){
		//the length of the vector
		return Math.sqrt(this.x*this.x+this.y*this.y+this.z*this.z);
	}
	public function crossp(b:V3){
		//cross product
		return new V3(this.y*b.z-this.z*b.y, this.z*b.x-this.x*b.z, this.x*b.y-this.y*b.x);
	}
	public function dotp(b:V3){
		//dot product
		return this.x*b.x+this.y*b.y+this.z*b.z;
	}
	public function print(){
		//for debbuging, display the vector
		return Math.round(this.x*100)/100.+" "+Math.round(this.y*100)/100.+" "+Math.round(this.z*100)/100.;
	}
}

class Loader extends Sprite{
	//This is the loading animation
	private var active:Int;
	private var color1:Int;
	private var color2:Int;
	private var numdots:Int;
	private var milliseconds:Int;
	private var separation:Int;
	private var timer:flash.utils.Timer;
	public function new(?color1:Int=-1,?color2:Int=-1){
		super();
		this.active=0;
		this.color1=color1<0?0x884400:color1; //The color of the active dot
		this.color2=color2<0?0x888888:color2; //The color of the inactive dots
		this.numdots=3; //The number of dots in the animation
		this.separation=16; //The separation in pixels of the dots
		this.milliseconds=800; //The number of milliseconds between each tick
		this.timer=new flash.utils.Timer(milliseconds,0); //Timer thats ticks every * milliseconds, forever
		this.timer.addEventListener(flash.events.TimerEvent.TIMER,render);
		timer.start();
		render(null);
	}
	private function render(evt:flash.events.TimerEvent){
		this.active=(this.active+1)%this.numdots;
		this.graphics.clear();
		
		for(i in 0...this.numdots){
			this.graphics.beginFill(this.active==i?this.color1:this.color2); //Choose the color
			this.graphics.drawCircle((i-this.numdots/2.)*this.separation,0,3); //Draw the dot
			this.graphics.endFill();
		}
	}
}
/*
class Quaternion{
	//This class defines a Quaternion and some operations
	public var u:Float;
	public var i:Float;
	public var j:Float;
	public var k:Float;
	
	public function new(u:Float,i:Float,j:Float,k:Float){
		this.u=u;this.i=i;this.j=j;this.k=k;
	}
	public function add(q:Quaternion){
		return new Quaternion(this.u*q.u,this.i*q.i,this.j*q.j,this.k*q.k);
	}
	public function mult(q:Quaternion){
		return new Quaternion(
			this.u*q.u - this.i*q.i - this.j*q.j - this.k*q.k,
			this.u*q.i + this.i*q.u + this.j*q.k - this.k*q.j,
			this.u*q.j - this.i*q.k + this.j*q.u + this.k*q.i,
			this.u*q.k + this.i*q.j - this.j*q.i + this.k*q.u
		);
	}
	public function conj(){
		return new Quaternion(this.u,-this.i,-this.j,-this.k);
	}
}*/
