class Sphere{
	static var stage:flash.display.MovieClip;
	static var service:String;
	static var user:String;
	static var album:String;
	static var loadin:flash.display.Sprite;
	static var thumbs:Array<Thumb>;
	static var framenum:Int;
	static var n:Int;
	static var fotos:flash.display.Sprite;
	static var r=100;
	static var phi:Float=0.003;
	static var theta:Float=0;
	static var mouseonstage=false;
	static var waiting=true;
	static var width=400;
	static var height=400;
	static var params:Dynamic<String>;
	static var thumbquality:Int;
	static var orient:V3;
	static var goto:V3;
	static var dbg:flash.display.Sprite;
	
	static function main(){
		params = flash.Lib.current.loaderInfo.parameters;
		service=params.service!=null?params.service:"picasa";
		user=params.user!=null?params.user:"kevinalle";
		album=params.album==null&&user=="kevinalle"?"StarredPhotos":params.album;
		thumbquality=params.thumbquality!=null?Std.parseInt(params.thumbquality):5;
		stage=flash.Lib.current;
		loadin=new flash.display.Sprite();
		thumbs=new Array();
		framenum=0;
		fotos=new flash.display.Sprite();
		orient=new V3(0,0,1);
		goto=new V3(0,0,1);
		
		var xmlLoader:flash.net.URLLoader=null;
		if(service=="picasa"){
			flash.system.Security.allowDomain("http://picasaweb.google.com");
			flash.system.Security.allowInsecureDomain("http://picasaweb.google.com");
			flash.system.Security.loadPolicyFile("http://photos.googleapis.com/data/crossdomain.xml");
			var thumbsizes=[32, 48, 64, 72, 104, 144, 150, 160];
			var thsz:Int=thumbquality>0&&thumbquality<=8?thumbquality:5;
			var url="http://photos.googleapis.com/data/feed/api/user/"+user+(album!=null?"/album/"+album:"")+"?thumbsize="+thumbsizes[thsz-1]+"c";
			xmlLoader = new flash.net.URLLoader(new flash.net.URLRequest(url));
		}else if(service=="custom"){
			xmlLoader = new flash.net.URLLoader(new flash.net.URLRequest(params.xml));
		}
		var tf = new flash.text.TextField();
		tf.text = "Loading...";
		loadin.addChild(tf);
		loadin.x=width/2-loadin.width/2;//stage.stage.stageWidth/2-loadin.width/2;
		loadin.y=height/2-loadin.height/2;//stage.stage.stageHeight/2-loadin.height/2;
		stage.addChild(loadin);
		xmlLoader.addEventListener(flash.events.Event.COMPLETE, axmlloaded);
		stage.stage.addEventListener(flash.events.Event.MOUSE_LEAVE,function(e:Dynamic){mouseonstage=false;});
		stage.stage.addEventListener(flash.events.MouseEvent.MOUSE_MOVE,function(e:Dynamic){mouseonstage=true;waiting=false;});
	}
	static function axmlloaded(evt:flash.events.Event){
		//trace("Loaded :P");
		stage.removeChild(loadin);
		var xml:flash.xml.XML=new flash.xml.XML(evt.target.data);
		var entrys:flash.xml.XMLList=null;
		var atom:flash.utils.Namespace =  new flash.utils.Namespace(xml.namespace());
		var gphoto:flash.utils.Namespace =  new flash.utils.Namespace("http://schemas.google.com/photos/2007");
		var media:flash.utils.Namespace =  new flash.utils.Namespace("http://search.yahoo.com/mrss/");
		if(service=="picasa"){
			entrys=xml.child( ns(atom,"entry") );
		}else if(service=="custom"){
			entrys=xml.child("photo");
		}
		n=entrys.length();
		if(params.max!=null){n=Math.round(Math.min(n,Std.parseInt(params.max)));}
		r=Math.floor(Math.min(width/2,height/2)*.7);//Math.floor(Math.min(stage.stage.stageWidth/2,stage.stage.stageHeight/2)*.8);
		
		for(i in 0...n){
			var thumb:String="";var url:String="";
			if(service=="picasa"){
				thumb=entrys[i].child(ns(media,"group"))[0].child(ns(media,"thumbnail"))[0].attribute("url").toString();
				url=entrys[i].child(ns(atom,"link"))[1].attribute("href").toString();
			}else if(service=="custom"){
				thumb=entrys[i].attribute("src").toString();
				url=entrys[i].attribute("href").toString();
			}
			thumbs.push(new Thumb(thumb,url));
			fotos.addChild(thumbs[i]);
		}
		shuffle(thumbs);
		fotos.addEventListener(flash.events.Event.ENTER_FRAME,frame);
		fotos.x=width/2;//stage.stage.stageWidth/2;
		fotos.y=height/2;//stage.stage.stageHeight/2;
		stage.addChild(fotos);
		
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
		sortFotos();
		
		
		for(i in 0...n){
			thumbs[i].display(thumbs[i].pos,n,r);
		}
		sortFotos();
		/*dbg=new flash.display.Sprite();
		dbg.x=width/2;
		dbg.y=height/2;
		stage.addChild(dbg);*/
	}
	
	static function frame(evt:flash.events.Event){
		framenum++;
		if(mouseonstage){
			theta=-fotos.mouseY/7000.;
			phi=-fotos.mouseX/7000.;
		}else if(!waiting){
			theta*=.9;
			phi*=.9;
		}
		var st=Math.sin(theta);
		var ct=Math.cos(theta);
		var sp=Math.sin(phi);
		var cp=Math.cos(phi);
		for(i in 0...n){
			var x=thumbs[i].pos.x;
			var y=ct*thumbs[i].pos.y+st*thumbs[i].pos.z;
			var z=-st*thumbs[i].pos.y+ct*thumbs[i].pos.z;
			var x2=cp*x+sp*z;
			var y2=y;
			var z2=-sp*x+cp*z;
			thumbs[i].pos=new V3(x2,y2,z2);
			thumbs[i].display(new V3(x2,y2,z2),n,r);
			thumbs[i].z=z2;
		}
		if((phi!=0||theta!=0) && framenum%10==0) sortFotos();
	}
	
	static function ns(namespace:flash.utils.Namespace, name:String){
		return new flash.utils.QName(namespace,new flash.utils.QName(name));
	}
	
	static function sortFotos(){
		thumbs.sort(function(a,b:Thumb){return a.z==b.z?0:a.z>b.z?1:-1;});
		var i=n;
		while(--i>=0) fotos.setChildIndex(thumbs[i], i);
	}
	
	static function shuffle<T>(arr:Array<T>){
		var n = arr.length;
		while (n > 1){
			var k = Std.random(n);
			n--;
			var temp = arr[n];
			arr[n] = arr[k];
			arr[k]= temp;
		}
	}
	
/*	static function axisrotate(as:V3,th:Float,xyz:V3){
		var axis:V3=as.normalized();
		var p=new Quaternion(0, xyz.x, xyz.y, xyz.z);
		var c=Math.cos(th/2.);var s = Math.sin(th/2.);
		var rot=new Quaternion(c, axis.x*s, axis.y*s, axis.z*s);
		var res=rot.mult(p).mult(rot.conj());
		return new V3(res.i,res.j,res.k);
		/*var ux=axis.x;var uy=axis.y;var uz=axis.z;
		return new V3(
			(ux*ux+c*(1-ux*ux))*x + (ux*uy*(1-c)-uz*s)*y + (ux*uz*(1-c)+uy*s)*z,
			(ux*uy*(1-c)+uz*s)*x + (uy*uy+c*(1-uy*uy))*y + (uy*uz*(1-c)-ux*s)*z,
			(ux*uz*(1-c)-uy*s)*x + (uy*uz*(1-c)+ux*s)*y + (uz*uz+c*(1-uz*uz))*z
		);* /
	}*/
}

class Thumb extends flash.display.Sprite{
	//public var img:flash.display.Sprite;
	public var link:String;
	public var pos:V3;
	public var z:Float;
	
	public function new(url:String,?link:String){
		super();
		var ldr:flash.display.Loader = new flash.display.Loader();
		ldr.name="loader";
		ldr.load(new flash.net.URLRequest(url));
		ldr.contentLoaderInfo.addEventListener(flash.events.Event.COMPLETE,this.loaded);
		//this.img=new flash.display.Sprite();
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
		if(w>h){ h=h*72/w;w=72; }
		else{ w=w*72/h;h=72; }
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
		this.scaleX=this.scaleY=(4.5/Math.sqrt(n))*(.8*pos.z/(2.*r)+.7)*(4./3);
	}
	
	function over(evt:flash.events.MouseEvent){
		var rect=new flash.display.Sprite();
		rect.graphics.beginFill(0,0);
		rect.graphics.drawRect(-1.2*this.width/2,-1.2*this.height/2,1.2*this.width,1.2*this.height);
		rect.name="rect";
		this.addChild(rect);
		this.pos.scaleBy(1.1);
		this.addEventListener(flash.events.MouseEvent.MOUSE_OUT,out);
	}
	function out(evt:flash.events.MouseEvent){
		this.removeChild(this.getChildByName("rect"));
		this.removeEventListener(flash.events.MouseEvent.MOUSE_OUT,out);
		this.pos.scaleBy(1/1.1);
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
	public var x:Float;
	public var y:Float;
	public var z:Float;
	public function new(x:Float,y:Float,z:Float){
		this.x=x;
		this.y=y;
		this.z=z;
	}
	public function normalize(?r:Float=1){
		var norm=Math.sqrt(this.x*this.x+this.y*this.y+this.z*this.z);
		this.x*=r/norm;
		this.y*=r/norm;
		this.z*=r/norm;
	}
	public function normalized(?r:Float=1){
		var norm=Math.sqrt(this.x*this.x+this.y*this.y+this.z*this.z);
		return new V3(this.x*r/norm, this.y*r/norm, this.z*r/norm);
	}
	public function add(v:V3){
		return new V3(this.x+v.x,this.y+v.y,this.z+v.z);
	}
	public function subtract(v:V3){
		return new V3(this.x-v.x,this.y-v.y,this.z-v.z);
	}
	public function scaleBy(s:Float){
		this.x*=s;
		this.y*=s;
		this.z*=s;
	}
	public function scaledBy(s:Float){
		return new V3(this.x*s,this.y*s,this.z*s);
	}
	public function norm(){
		return Math.sqrt(this.x*this.x+this.y*this.y+this.z*this.z);
	}
	public function crossp(b:V3){
		return new V3(this.y*b.z-this.z*b.y, this.z*b.x-this.x*b.z, this.x*b.y-this.y*b.x);
	}
	public function dotp(b:V3){
		return this.x*b.x+this.y*b.y+this.z*b.z;
	}
	public function print(){
		return Math.round(this.x*100)/100.+" "+Math.round(this.y*100)/100.+" "+Math.round(this.z*100)/100.;
	}
}
/*
class Quaternion{
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
