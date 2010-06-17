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
	
	static function main(){
		user="kevinalle";
		album="100612_EscaladaDisfuntos";
		stage=flash.Lib.current;
		loadin=new flash.display.Sprite();
		thumbs=new Array();
		framenum=0;
		fotos=new flash.display.Sprite();
		
		flash.system.Security.allowDomain("http://picasaweb.google.com");
		flash.system.Security.allowInsecureDomain("http://picasaweb.google.com");
		flash.system.Security.loadPolicyFile("http://photos.googleapis.com/data/crossdomain.xml");
		var xmlLoader:flash.net.URLLoader = new flash.net.URLLoader(new flash.net.URLRequest("http://photos.googleapis.com/data/feed/api/user/"+user+"/album/"+album+"?thumbsize=72c"));
		//var xmlLoader:flash.net.URLLoader = new flash.net.URLLoader(new flash.net.URLRequest("picasa.xml"));
		var tf = new flash.text.TextField();
		tf.text = "Loading...";
		loadin.addChild(tf);
		loadin.x=stage.stage.stageWidth/2-loadin.width/2;
		loadin.y=stage.stage.stageHeight/2-loadin.height/2;
		stage.addChild(loadin);
		xmlLoader.addEventListener(flash.events.Event.COMPLETE, axmlloaded);
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
		for(i in 0...n){
			//var tit:String=entrys[i].child(ns(atom,"title"))[0].toString();
			var thumb:String=entrys[i].child(ns(media,"group"))[0].child(ns(media,"thumbnail"))[0].attribute("url").toString();
			//trace(thumb);
			thumbs.push(new Thumb(thumb));
			fotos.addChild(thumbs[i].img);
		}
		fotos.addEventListener(flash.events.Event.ENTER_FRAME,frame);
		fotos.x=stage.stage.stageWidth/2;
		fotos.y=stage.stage.stageHeight/2;
		stage.addChild(fotos);
		for(i in 0...n){
			thumbs[i].px=Math.cos(2*Math.PI*i/n)*r;
			thumbs[i].py=Math.random()*200-100;
			thumbs[i].pz=Math.sin(2*Math.PI*i/n)*r;
		}
	}
	
	static function frame(evt:flash.events.Event){
		framenum++;
		phi-=fotos.mouseX/1000.;
		theta-=fotos.mouseY/1000.;
		for(i in 0...n){
			var st=Math.sin(theta);
			var ct=Math.cos(theta);
			var sp=Math.sin(phi);
			var cp=Math.cos(phi);
			/*var x=cp*thumbs[i].px+sp*thumbs[i].pz;
			var y=-st*sp*thumbs[i].px+ct*thumbs[i].py+cp*st*thumbs[i].pz;
			var z=-ct*sp*thumbs[i].px-st*thumbs[i].py+ct*cp*thumbs[i].pz;*/
			var x=thumbs[i].px;
			var y=ct*thumbs[i].py+st*thumbs[i].pz;
			var z=-st*thumbs[i].py+ct*thumbs[i].pz;
			var x2=cp*x+sp*z;
			var y2=y;
			var z2=-sp*x+cp*z;
			thumbs[i].img.x=x2;//thumbs[i].px;
			thumbs[i].img.y=y2;//thumbs[i].py;
			thumbs[i].img.alpha=z2/(2.*r)+0.5;//thumbs[i].pz/(2.*r)+0.5;
			thumbs[i].img.scaleX=thumbs[i].img.scaleY=.8*z2/(2.*r)+.7;
			sortFotos();
		}
	}
	
	static function ns(namespace:flash.utils.Namespace, name:String){
		return new flash.utils.QName(namespace,new flash.utils.QName(name));
	}
	
	static function sortFotos(){
		thumbs.sort(function(a,b:Thumb){return a.img.alpha>b.img.alpha?1:-1;});
		var i=n-1;
		while(i>=0){
			fotos.setChildIndex(thumbs[i].img, i);
			i--;
		}
	}
}

class Thumb{
	public var img:flash.display.Sprite;
	public var px:Float;
	public var py:Float;
	public var pz:Float;
	
	public function new(url:String){
		var ldr:flash.display.Loader = new flash.display.Loader();
		ldr.name="loader";
		ldr.load(new flash.net.URLRequest(url));
		ldr.contentLoaderInfo.addEventListener(flash.events.Event.COMPLETE,this.loaded);
		this.img=new flash.display.Sprite();
		this.img.addChild(ldr);
	}
	
	private function loaded(?evt:flash.events.Event){
		var ldr=this.img.getChildByName("loader");
		ldr.x=-ldr.width/2;
		ldr.y=-ldr.height/2;
	}
}
