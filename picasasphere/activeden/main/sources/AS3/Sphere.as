package  {
	import flash.net.URLRequest;
	import flash.system.Security;
	import flash.events.MouseEvent;
	import flash.display.MovieClip;
	import flash.Lib;
	import flash.text.TextField;
	import flash.display.Sprite;
	import flash.utils.QName;
	import flash.utils.Namespace;
	import flash.events.Event;
	import flash.net.URLLoader;
	public class Sphere {
		static protected var stage : flash.display.MovieClip;
		static protected var service : String;
		static protected var user : String;
		static protected var album : String;
		static protected var loadin : flash.display.Sprite;
		static protected var thumbs : Array;
		static protected var framenum : int;
		static protected var n : int;
		static protected var fotos : flash.display.Sprite;
		static protected var r : int = 100;
		static protected var phi : Number = 0.003;
		static protected var theta : Number = 0;
		static protected var mouseonstage : Boolean = false;
		static protected var waiting : Boolean = true;
		static protected var width : int = 400;
		static protected var height : int = 400;
		static protected var params : *;
		static protected var thumbquality : int;
		static protected var orient : V3;
		static protected var goto : V3;
		static protected var dbg : flash.display.Sprite;
		static public function main() : void {
			Sphere.params = flash.Lib.current.loaderInfo.parameters;
			Sphere.service = (params.service != null?params.service:"picasa");
			Sphere.user = (params.user != null?params.user:"kevinalle");
			Sphere.album = (params.album == null && Sphere.user == "kevinalle"?"StarredPhotos":params.album);
			Sphere.thumbquality = (params.thumbquality != null?Std._parseInt(params.thumbquality):5);
			Sphere.stage = flash.Lib.current;
			Sphere.loadin = new flash.display.Sprite();
			Sphere.thumbs = new Array();
			Sphere.framenum = 0;
			Sphere.fotos = new flash.display.Sprite();
			Sphere.orient = new V3(0,0,1);
			Sphere.goto = new V3(0,0,1);
			var xmlLoader : flash.net.URLLoader = null;
			if(Sphere.service == "picasa") {
				flash.system.Security.allowDomain("http://picasaweb.google.com");
				flash.system.Security.allowInsecureDomain("http://picasaweb.google.com");
				flash.system.Security.loadPolicyFile("http://photos.googleapis.com/data/crossdomain.xml");
				var thumbsizes : Array = [32,48,64,72,104,144,150,160];
				var thsz : int = (Sphere.thumbquality > 0 && Sphere.thumbquality <= 8?thumbquality:5);
				var url : String = "http://photos.googleapis.com/data/feed/api/user/" + user + ((Sphere.album != null?"/album/" + album:"")) + "?thumbsize=" + thumbsizes[thsz - 1] + "c";
				xmlLoader = new flash.net.URLLoader(new flash.net.URLRequest(url));
			}
			else if(Sphere.service == "custom") {
				xmlLoader = new flash.net.URLLoader(new flash.net.URLRequest(params.xml));
			}
			var tf : flash.text.TextField = new flash.text.TextField();
			tf.text = "Loading...";
			loadin.addChild(tf);
			loadin.x = Sphere.width / 2 - loadin.width / 2;
			loadin.y = Sphere.height / 2 - loadin.height / 2;
			stage.addChild(loadin);
			xmlLoader.addEventListener(flash.events.Event.COMPLETE,Sphere.axmlloaded);
			stage.stage.addEventListener(flash.events.Event.MOUSE_LEAVE,function(e : *) : void {
				Sphere.mouseonstage = false;
			});
			stage.stage.addEventListener(flash.events.MouseEvent.MOUSE_MOVE,function(e : *) : void {
				Sphere.mouseonstage = true;
				Sphere.waiting = false;
			});
		}
		
		static protected function axmlloaded(evt : flash.events.Event) : void {
			stage.removeChild(loadin);
			var xml : XML = new XML(evt.target.data);
			var entrys : XMLList = null;
			var atom : flash.utils.Namespace = new flash.utils.Namespace(xml._namespace());
			var gphoto : flash.utils.Namespace = new flash.utils.Namespace("http://schemas.google.com/photos/2007");
			var media : flash.utils.Namespace = new flash.utils.Namespace("http://search.yahoo.com/mrss/");
			if(Sphere.service == "picasa") {
				entrys = xml.child(ns(atom,"entry"));
			}
			else if(Sphere.service == "custom") {
				entrys = xml.child("photo");
			}
			Sphere.n = entrys.length();
			if(params.max != null) {
				Sphere.n = Math.round(Math.min(n,Std._parseInt(params.max)));
			}
			Sphere.r = Math.floor(Math.min(Sphere.width / 2,Sphere.height / 2) * .7);
			{
				var _g1 : int = 0, _g : int = n;
				while(_g1 < _g) {
					var i : int = _g1++;
					var thumb : String = "";
					var url : String = "";
					if(Sphere.service == "picasa") {
						thumb = entrys[i].child(ns(media,"group"))[0].child(ns(media,"thumbnail"))[0].attribute("url").toString();
						url = entrys[i].child(ns(atom,"link"))[1].attribute("href").toString();
					}
					else if(Sphere.service == "custom") {
						thumb = entrys[i].attribute("src").toString();
						url = entrys[i].attribute("href").toString();
					}
					thumbs.push(new Thumb(thumb,url));
					fotos.addChild(thumbs[i]);
				}
			}
			shuffle(thumbs);
			fotos.addEventListener(flash.events.Event.ENTER_FRAME,Sphere.frame);
			fotos.x = Sphere.width / 2;
			fotos.y = Sphere.height / 2;
			stage.addChild(fotos);
			var s : Number = 3.6 / Math.sqrt(n);
			var long : Number = 0.;
			var dz : Number = 2. / n;
			var z : Number = 1 - dz / 2;
			{
				var _g12 : int = 0, _g2 : int = n;
				while(_g12 < _g2) {
					var k : int = _g12++;
					var rr : Number = Math.sqrt(1 - z * z);
					thumbs[k].pos = new V3(Math.cos(long) * rr * r,Math.sin(long) * rr * r,z * r);
					thumbs[k].z = z * r;
					z -= dz;
					long += s / rr;
				}
			}
			sortFotos();
			{
				var _g13 : int = 0, _g3 : int = n;
				while(_g13 < _g3) {
					var i2 : int = _g13++;
					thumbs[i2].display(thumbs[i2].pos,n,r);
				}
			}
			sortFotos();
		}
		
		static protected function frame(evt : flash.events.Event) : void {
			framenum++;
			if(mouseonstage) {
				Sphere.theta = -fotos.mouseY / 7000.;
				Sphere.phi = -fotos.mouseX / 7000.;
			}
			else if(!waiting) {
				Sphere.theta *= .9;
				Sphere.phi *= .9;
			}
			var st : Number = Math.sin(theta);
			var ct : Number = Math.cos(theta);
			var sp : Number = Math.sin(phi);
			var cp : Number = Math.cos(phi);
			{
				var _g1 : int = 0, _g : int = n;
				while(_g1 < _g) {
					var i : int = _g1++;
					var x : Number = thumbs[i].pos.x;
					var y : Number = ct * thumbs[i].pos.y + st * thumbs[i].pos.z;
					var z : Number = -st * thumbs[i].pos.y + ct * thumbs[i].pos.z;
					var x2 : Number = cp * x + sp * z;
					var y2 : Number = y;
					var z2 : Number = -sp * x + cp * z;
					thumbs[i].pos = new V3(x2,y2,z2);
					thumbs[i].display(new V3(x2,y2,z2),n,r);
					thumbs[i].z = z2;
				}
			}
			if((Sphere.phi != 0 || Sphere.theta != 0) && Sphere.framenum % 10 == 0) sortFotos();
		}
		
		static protected function ns(_namespace : flash.utils.Namespace,name : String) : flash.utils.QName {
			return new flash.utils.QName(_namespace,new flash.utils.QName(name));
		}
		
		static protected function sortFotos() : void {
			thumbs.sort(function(a : Thumb,b : Thumb) : int {
				return (a.z == b.z?0:(a.z > b.z?1:-1));
			});
			var i : int = n;
			while(--i >= 0) fotos.setChildIndex(thumbs[i],i);
		}
		
		static protected function shuffle(arr : Array) : void {
			var n : int = arr.length;
			while(n > 1) {
				var k : int = Std.random(n);
				n--;
				var temp : * = arr[n];
				arr[n] = arr[k];
				arr[k] = temp;
			}
		}
		
	}
}
