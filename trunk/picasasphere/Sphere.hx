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
	static var vt:Float=0;
	static var vp:Float=0.003;
	static var mouseonstage=false;
	static var waiting=true;
	static var fixed=false;
	static var width=300;
	static var height=300;
	static var params:Dynamic<String>;
	static var thumbquality:Int;
	static var orient:V3;
	static var goto:V3;
	
	static function main(){
		params = flash.Lib.current.loaderInfo.parameters;
		user=params.user!=null?params.user:"kevinalle";
		album=params.album;
		thumbquality=params.thumbquality!=null?Std.parseInt(params.thumbquality):5;
		stage=flash.Lib.current;
		loadin=new flash.display.Sprite();
		thumbs=new Array();
		framenum=0;
		fotos=new flash.display.Sprite();
		orient=new V3(0,0,1);
		goto=new V3(1,0,0);
		
		flash.system.Security.allowDomain("http://picasaweb.google.com");
		flash.system.Security.allowInsecureDomain("http://picasaweb.google.com");
		flash.system.Security.loadPolicyFile("http://photos.googleapis.com/data/crossdomain.xml");
		var thumbsizes=[32, 48, 64, 72, 104, 144, 150, 160];
		var thsz:Int=thumbquality>0&&thumbquality<=8?thumbquality:5;
		var url="http://photos.googleapis.com/data/feed/api/user/"+user+(album!=null?"/album/"+album:"")+"?thumbsize="+thumbsizes[thsz-1]+"c";
		var xmlLoader:flash.net.URLLoader = new flash.net.URLLoader(new flash.net.URLRequest(url));
		//var xmlLoader:flash.net.URLLoader = new flash.net.URLLoader(new flash.net.URLRequest("picasa.xml"));
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
		var atom:flash.utils.Namespace =  new flash.utils.Namespace(xml.namespace());
		var gphoto:flash.utils.Namespace =  new flash.utils.Namespace("http://schemas.google.com/photos/2007");
		var media:flash.utils.Namespace =  new flash.utils.Namespace("http://search.yahoo.com/mrss/");
		var entrys:flash.xml.XMLList=xml.child( ns(atom,"entry") );
		//trace(xml.child(ns(atom,"entry")));
		//trace(entrys.length());
		n=entrys.length();
		if(params.max!=null){n=Math.round(Math.min(n,Std.parseInt(params.max)));}
		r=Math.floor(Math.min(width/2,height/2)*.75);//Math.floor(Math.min(stage.stage.stageWidth/2,stage.stage.stageHeight/2)*.8);
		for(i in 0...n){
			//var tit:String=entrys[i].child(ns(atom,"title"))[0].toString();
			var thumb:String=entrys[i].child(ns(media,"group"))[0].child(ns(media,"thumbnail"))[0].attribute("url").toString();
			var url:String=entrys[i].child(ns(atom,"link"))[1].attribute("href").toString();
			//trace(thumb);
			thumbs.push(new Thumb(thumb,url));
			if(fixed) thumbs[i].img.addEventListener(flash.events.MouseEvent.MOUSE_OVER,function(evt:flash.events.MouseEvent){ goto=thumbs[i].pos.normalized(); });
			fotos.addChild(thumbs[i].img);
		}
		shuffle(thumbs);
		fotos.addEventListener(flash.events.Event.ENTER_FRAME,frame);
		fotos.x=width/2;//stage.stage.stageWidth/2;
		fotos.y=height/2;//stage.stage.stageHeight/2;
		//trace(fotos.x);
		stage.addChild(fotos);
		//Spiral points on sphere: http://sitemason.vanderbilt.edu/page/hmbADS#spiral
		var s = 3.6/Math.sqrt(n);
		var long = 0.;
		var dz = 2.0/n;
		var z = 1 - dz/2;
		for(k in 0...n){
			var rr = Math.sqrt(1-z*z);
			thumbs[k].pos = new V3(Math.cos(long)*rr*r, Math.sin(long)*rr*r, z*r);
			z = z - dz;
			long = long + s/rr;
		}
	}
	
	static function frame(evt:flash.events.Event){
		framenum++;
		if(fixed){
			/*phi=-fotos.mouseX/80.;
			theta=-fotos.mouseY/80.;*/
			
			//goto=axisrotate(new V3(fotos.mouseY,-fotos.mouseX,0), fotos.mouseX*fotos.mouseX/100.+fotos.mouseY*fotos.mouseY/100., goto).normalized();
			//trace(goto.print());
			var axis=orient.crossp(goto).normalized();
			var th=Math.asin(axis.norm());
			orient=axisrotate(axis,th,orient).normalized();
			if(th>.001){
				for(i in 0...n){
					var xyz=axisrotate(axis,th,thumbs[i].pos);
					thumbs[i].display(xyz,n,r);
				}
				if((Math.abs(vp)>0.0001||Math.abs(vt)>0.0001) && framenum%10==0) sortFotos();
			}


		}else{
			if(mouseonstage){
				vt=-fotos.mouseY/10000.;
				vp=-fotos.mouseX/10000.;
			}else if(!waiting){
				vt*=.9;
				vp*=.9;
			}
			phi+=vp;//(mouseonstage?1:0)*fotos.mouseX/5000.;
			theta+=vt;//(mouseonstage?1:0)*fotos.mouseY/5000.;
		
			//phi=theta=0;
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
			
				//var xyz2=axisrotate(new V3(1,0,0),-theta, thumbs[i].px,thumbs[i].py,thumbs[i].pz);
				//var xyz=axisrotate(new V3(0,1,0),phi, xyz2.x,xyz2.y,xyz2.z);
				//var xyz=axisrotate(new V3(-theta,phi,0),Math.sqrt(phi*phi+theta*theta), thumbs[i].px,thumbs[i].py,thumbs[i].pz);
				/*thumbs[i].img.x=xyz.x;//thumbs[i].px;
				thumbs[i].img.y=xyz.y;//thumbs[i].py;
				thumbs[i].img.alpha=xyz.z/(3.*r)+.66;//thumbs[i].pz/(2.*r)+0.5;
				thumbs[i].img.scaleX=thumbs[i].img.scaleY=(4.5/Math.sqrt(n))*(.8*xyz.z/(2.*r)+.7);*/
				thumbs[i].display(new V3(x2,y2,z2),n,r);
			}
			if((Math.abs(vp)>0.0001||Math.abs(vt)>0.0001) && framenum%10==0) sortFotos();
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
	
	static function axisrotate(axis:V3,th:Float,xyz:V3){
		axis.normalize();
		var x=xyz.x;
		var y=xyz.y;
		var z=xyz.z;
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
	public var pos:V3;
	
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
		this.img.addEventListener(flash.events.MouseEvent.MOUSE_OVER,over);
	}
	
	private function loaded(?evt:flash.events.Event){
		var ldr=this.img.getChildByName("loader");
		var w=ldr.width;var h=ldr.height;
		if(w>h){ h=h*72/w;w=72; }
		else{ w=w*72/h;h=72; }
		ldr.width=w;ldr.height=h;
		var sze=ldr.width>ldr.height?ldr.width:ldr.height;
		img.buttonMode=true;
		img.graphics.beginFill(0xffffff);
		img.graphics.drawRect(-sze/2-4,-sze/2-4,sze+8,sze+8);
		img.mouseChildren=false;
		ldr.x=-ldr.width/2;
		ldr.y=-ldr.height/2;
	}
	
	public function display(pos:V3,n:Int,r:Float){
		this.img.x=pos.x;
		this.img.y=pos.y;
		this.img.alpha=pos.z/(3.*r)+.66;
		this.img.scaleX=this.img.scaleY=(4.5/Math.sqrt(n))*(.8*pos.z/(2.*r)+.7);
	}
	
	function over(evt:flash.events.MouseEvent){
		var rect=new flash.display.Sprite();
		rect.graphics.beginFill(0,0);
		rect.graphics.drawRect(-1.2*img.width/2,-1.2*img.height/2,1.2*img.width,1.2*img.height);
		rect.name="rect";
		this.img.addChild(rect);
		this.pos.scaleBy(1.1);
		this.img.addEventListener(flash.events.MouseEvent.MOUSE_OUT,out);
	}
	function out(evt:flash.events.MouseEvent){
		this.img.removeChild(this.img.getChildByName("rect"));
		this.img.removeEventListener(flash.events.MouseEvent.MOUSE_OUT,out);
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
	public function print(){
		return this.x+" "+this.y+" "+this.z;
	}
}
