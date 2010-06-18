class Sphere{
	static var stage:flash.display.MovieClip;
	static var user:String;
	static var album:String;
	static var loadin:flash.display.Sprite;
	static var thumbs:Array<Thumb>;
	static var framenum:Int;
	static var n:Int;
	static var fotos:flash.display.Sprite;
	static var r=100;
	static var phi:Float=0;
	static var theta:Float=0;
	static var mouseonstage=false;
	static var fixed=false;
	
	static function main(){
		var params:Dynamic<String> = flash.Lib.current.loaderInfo.parameters;
		user=params.user!=null?params.user:"kevinalle";
		album=params.album;
		stage=flash.Lib.current;
		loadin=new flash.display.Sprite();
		thumbs=new Array();
		framenum=0;
		fotos=new flash.display.Sprite();
		
		flash.system.Security.allowDomain("http://picasaweb.google.com");
		flash.system.Security.allowInsecureDomain("http://picasaweb.google.com");
		flash.system.Security.loadPolicyFile("http://photos.googleapis.com/data/crossdomain.xml");
		var url=album!=null?"http://photos.googleapis.com/data/feed/api/user/"+user+"/album/"+album+"?thumbsize=72c":"http://photos.googleapis.com/data/feed/api/user/"+user+"?thumbsize=72c";
		var xmlLoader:flash.net.URLLoader = new flash.net.URLLoader(new flash.net.URLRequest(url));
		//var xmlLoader:flash.net.URLLoader = new flash.net.URLLoader(new flash.net.URLRequest("picasa.xml"));
		var tf = new flash.text.TextField();
		tf.text = "Loading...";
		loadin.addChild(tf);
		loadin.x=stage.stage.stageWidth/2-loadin.width/2;
		loadin.y=stage.stage.stageHeight/2-loadin.height/2;
		stage.addChild(loadin);
		xmlLoader.addEventListener(flash.events.Event.COMPLETE, axmlloaded);
		stage.stage.addEventListener(flash.events.Event.MOUSE_LEAVE,function(e:Dynamic){mouseonstage=false;});
		stage.stage.addEventListener(flash.events.MouseEvent.MOUSE_MOVE,function(e:Dynamic){mouseonstage=true;});
	}
	static function axmlloaded(evt:flash.events.Event){
		//trace("Loaded :P");
		stage.removeChild(loadin);
		var xml:flash.xml.XML=new flash.xml.XML(evt.target.data);
		var atom:flash.utils.Namespace =  new flash.utils.Namespace(xml.namespace());
		var gphoto:flash.utils.Namespace =  new flash.utils.Namespace("http://schemas.google.com/photos/2007");
		var media:flash.utils.Namespace =  new flash.utils.Namespace("http://search.yahoo.com/mrss/");
		var entrys:flash.xml.XMLList=xml.child( ns(atom,"entry") );
		//trace(xml.child(ns(atom,"entry")));
		//trace(entrys.length());
		n=entrys.length();
		r=Math.floor(Math.min(stage.stage.stageWidth/2,stage.stage.stageHeight/2)*.8);
		for(i in 0...n){
			//var tit:String=entrys[i].child(ns(atom,"title"))[0].toString();
			var thumb:String=entrys[i].child(ns(media,"group"))[0].child(ns(media,"thumbnail"))[0].attribute("url").toString();
			var url:String=entrys[i].child(ns(atom,"link"))[1].attribute("href").toString();
			//trace(thumb);
			thumbs.push(new Thumb(thumb,url));
			fotos.addChild(thumbs[i].img);
		}
		shuffle(thumbs);
		fotos.addEventListener(flash.events.Event.ENTER_FRAME,frame);
		fotos.x=stage.stage.stageWidth/2;
		fotos.y=stage.stage.stageHeight/2;
		stage.addChild(fotos);
		//Spiral points on sphere: http://sitemason.vanderbilt.edu/page/hmbADS#spiral
		var s = 3.6/Math.sqrt(n);
		var long = 0.;
		var dz = 2.0/n;
		var z = 1 - dz/2;
		for(k in 0...n-1){
			var rr = Math.sqrt(1-z*z);
			thumbs[k].px = Math.cos(long)*rr*r;
			thumbs[k].py = Math.sin(long)*rr*r;
			thumbs[k].pz = z*r;
			z = z - dz;
			long = long + s/rr;
		}
	}
	
	static function frame(evt:flash.events.Event){
		//framenum++;
		if(fixed){
			phi=-(mouseonstage?1:0)*fotos.mouseX/50.;
			theta=-(mouseonstage?1:0)*fotos.mouseY/50.;
		}else{
			phi-=(mouseonstage?1:0)*fotos.mouseX/5000.;
			theta-=(mouseonstage?1:0)*fotos.mouseY/5000.;
		}
		//phi=theta=0;
		var st=Math.sin(theta);
		var ct=Math.cos(theta);
		var sp=Math.sin(phi);
		var cp=Math.cos(phi);
		for(i in 0...n){
			var x=thumbs[i].px;
			var y=ct*thumbs[i].py+st*thumbs[i].pz;
			var z=-st*thumbs[i].py+ct*thumbs[i].pz;
			var x2=cp*x+sp*z;
			var y2=y;
			var z2=-sp*x+cp*z;
			
			//var xyz2=axisrotate(new V3(1,0,0),-theta, thumbs[i].px,thumbs[i].py,thumbs[i].pz);
			//var xyz=axisrotate(new V3(0,1,0),phi, xyz2.x,xyz2.y,xyz2.z);
			//var xyz=axisrotate(new V3(-theta,phi,0),Math.sqrt(phi*phi+theta*theta), thumbs[i].px,thumbs[i].py,thumbs[i].pz);
			/*thumbs[i].img.x=xyz.x;//thumbs[i].px;
			thumbs[i].img.y=xyz.y;//thumbs[i].py;
			thumbs[i].img.alpha=xyz.z/(3.*r)+.66;//thumbs[i].pz/(2.*r)+0.5;
			thumbs[i].img.scaleX=thumbs[i].img.scaleY=(4.5/Math.sqrt(n))*(.8*xyz.z/(2.*r)+.7);*/
			thumbs[i].img.x=x2;
			thumbs[i].img.y=y2;
			thumbs[i].img.alpha=z2/(3.*r)+.66;
			thumbs[i].img.scaleX=thumbs[i].img.scaleY=(4.5/Math.sqrt(n))*(.8*z2/(2.*r)+.7);
			sortFotos();
		}
	}
	
	static function ns(namespace:flash.utils.Namespace, name:String){
		return new flash.utils.QName(namespace,new flash.utils.QName(name));
	}
	
	static function sortFotos(){
		thumbs.sort(function(a,b:Thumb){return a.img.alpha==b.img.alpha?0:a.img.alpha>b.img.alpha?1:-1;});
		var i=n-1;
		while(--i>=0) fotos.setChildIndex(thumbs[i].img, i);
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
	
	static function axisrotate(axis:V3,th:Float,x:Float,y:Float,z:Float){
		axis.normalize();
		var c = Math.cos(th);var s = Math.sin(th);
		var ux=axis.x;var uy=axis.y;var uz=axis.z;
		return new V3(
			(ux*ux+c*(1-ux*ux))*x + (ux*uy*(1-c)-uz*s)*y + (ux*uz*(1-c)+uy*s)*z,
			(ux*uy*(1-c)+uz*s)*x + (uy*uy+c*(1-uy*uy))*y + (uy*uz*(1-c)-ux*s)*z,
			(ux*uz*(1-c)-uy*s)*x + (uy*uz*(1-c)+ux*s)*y + (uz*uz+c*(1-uz*uz))*z
		);
	}
}

class Thumb{
	public var img:flash.display.Sprite;
	public var link:String;
	public var px:Float;
	public var py:Float;
	public var pz:Float;
	
	public function new(url:String,?link:String){
		var ldr:flash.display.Loader = new flash.display.Loader();
		ldr.name="loader";
		ldr.load(new flash.net.URLRequest(url));
		ldr.contentLoaderInfo.addEventListener(flash.events.Event.COMPLETE,this.loaded);
		this.img=new flash.display.Sprite();
		this.img.rotation=(Math.random()-.5)*20;
		var drpShdw=new flash.filters.DropShadowFilter(2,45,0,1,8,8,.6,1);
		this.img.filters=[drpShdw];
		this.img.addChild(ldr);
		this.link=link;
		if(this.link!=null) this.img.addEventListener(flash.events.MouseEvent.CLICK,openurl);
	}
	
	private function loaded(?evt:flash.events.Event){
		var ldr=this.img.getChildByName("loader");
		var sze=ldr.width>ldr.height?ldr.width:ldr.height;
		img.graphics.beginFill(0xffffff);
		img.graphics.drawRect(-sze/2-4,-sze/2-4,sze+8,sze+8);
		ldr.x=-ldr.width/2;
		ldr.y=-ldr.height/2;
	}
	
	function openurl(evt:flash.events.MouseEvent){
		var url = new flash.net.URLRequest(this.link);
		flash.Lib.getURL(url, "_blank");
	}
	
	public function normalize(r:Float){
		var norm=Math.sqrt(this.px*this.px+this.py*this.py+this.pz*this.pz);
		this.px*=r/norm;
		this.py*=r/norm;
		this.pz*=r/norm;
	}
}

class V3{
	public var x:Float;
	public var y:Float;
	public var z:Float;
	public function new(x,y,z:Float){
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
}
