package  {
	import flash.net.URLRequest;
	import flash.system.Security;
	import flash.events.MouseEvent;
	import flash.display.Sprite;
	import flash.net.URLLoader;
	import flash.events.Event;
	import flash.Boot;
	public class Sphere extends flash.display.Sprite {
		public function Sphere(_service : String = "picasa",_user : String = "kevinalle",_album : String = "StarredPhotos",_thumbquality : int = 5,_xml : String = null,_max : int = 0) : void { if( !flash.Boot.skip_constructor ) {
			super();
			this.service = (_service != null?_service:"picasa");
			this.user = (_user != null?_user:"kevinalle");
			this.album = (_album == null && this.user == "kevinalle"?"StarredPhotos":_album);
			this.thumbquality = _thumbquality;
			this.max = _max;
			this.initialRotationSpeed = 0.003;
			this.rotationSpeed = 0.15;
			this.r = 100;
			this.phi = 0.003;
			this.theta = 0;
			this.mouseonstage = false;
			this.waiting = true;
			this._width = 400;
			this._height = 400;
			this.loadin = new Loader();
			this.thumbs = new Array();
			this.framenum = 0;
			this.photos = new flash.display.Sprite();
			var xmlLoader : flash.net.URLLoader = null;
			if(this.service == "picasa") {
				flash.system.Security.allowDomain("http://picasaweb.google.com");
				flash.system.Security.allowInsecureDomain("http://picasaweb.google.com");
				flash.system.Security.loadPolicyFile("http://photos.googleapis.com/data/crossdomain.xml");
				var thumbsizes : Array = [32,48,64,72,104,144,150,160];
				var thsz : int = (this.thumbquality > 0 && this.thumbquality <= 8?this.thumbquality:5);
				var url : String = "http://photos.googleapis.com/data/feed/api/user/" + this.user + ((this.album != null?"/album/" + this.album:"")) + "?thumbsize=" + thumbsizes[thsz - 1] + "c";
				xmlLoader = new flash.net.URLLoader(new flash.net.URLRequest(url));
			}
			else if(this.service == "custom") {
				xmlLoader = new flash.net.URLLoader(new flash.net.URLRequest(_xml));
			}
			this.loadin.x = this._width / 2;
			this.loadin.y = this._height / 2;
			this.graphics.beginFill(0,0);
			this.graphics.drawRect(0,0,this._width,this._height);
			this.graphics.endFill();
			this.addChild(this.loadin);
			xmlLoader.addEventListener(flash.events.Event.COMPLETE,this.xmlloaded);
			this.addEventListener(flash.events.MouseEvent.ROLL_OUT,this.mouseout);
			this.addEventListener(flash.events.MouseEvent.ROLL_OVER,this.mousemove);
		}}
		
		protected var initialRotationSpeed : Number;
		protected var rotationSpeed : Number;
		protected var service : String;
		protected var user : String;
		protected var album : String;
		protected var loadin : Loader;
		protected var thumbs : Array;
		protected var framenum : int;
		protected var n : int;
		protected var photos : flash.display.Sprite;
		protected var r : Number;
		protected var phi : Number;
		protected var theta : Number;
		protected var mouseonstage : Boolean;
		protected var waiting : Boolean;
		protected var _width : int;
		protected var _height : int;
		protected var thumbquality : int;
		protected var max : int;
		protected var orient : V3;
		protected var goto : V3;
		protected var dbg : flash.display.Sprite;
		protected function mouseout(e : flash.events.MouseEvent) : void {
			this.mouseonstage = false;
		}
		
		protected function mousemove(e : flash.events.MouseEvent) : void {
			this.mouseonstage = true;
			this.waiting = false;
		}
		
		protected function xmlloaded(evt : flash.events.Event) : void {
			this.removeChild(this.loadin);
			var xml : XML = new XML(evt.target.data);
			var entrys : XMLList = null;
			var atom : Namespace = new Namespace(xml.namespace());
			var gphoto : Namespace = new Namespace("http://schemas.google.com/photos/2007");
			var media : Namespace = new Namespace("http://search.yahoo.com/mrss/");
			if(this.service == "picasa") {
				entrys = xml.child(this.ns(atom,"entry"));
			}
			else if(this.service == "custom") {
				entrys = xml.child("photo");
			}
			this.n = entrys.length();
			if(this.max > 0) {
				this.n = Math.round(Math.min(this.n,this.max));
			}
			this.r = Math.floor(Math.min(this._width / 2,this._height / 2) * .7);
			{
				var _g1 : int = 0, _g : int = this.n;
				while(_g1 < _g) {
					var i : int = _g1++;
					var thumb : String = "";
					var url : String = "";
					if(this.service == "picasa") {
						thumb = entrys[i].child(this.ns(media,"group"))[0].child(this.ns(media,"thumbnail"))[0].attribute("url").toString();
						url = entrys[i].child(this.ns(atom,"link"))[1].attribute("href").toString();
					}
					else if(this.service == "custom") {
						thumb = entrys[i].attribute("src").toString();
						url = entrys[i].attribute("href").toString();
					}
					this.thumbs.push(new Thumb(thumb,url));
					this.photos.addChild(this.thumbs[i]);
				}
			}
			this.shuffle(this.thumbs);
			this.photos.addEventListener(flash.events.Event.ENTER_FRAME,this.frame);
			this.photos.x = this._width / 2;
			this.photos.y = this._height / 2;
			this.addChild(this.photos);
			var s : Number = 3.6 / Math.sqrt(this.n);
			var long : Number = 0.;
			var dz : Number = 2. / this.n;
			var z : Number = 1 - dz / 2;
			{
				var _g12 : int = 0, _g2 : int = this.n;
				while(_g12 < _g2) {
					var k : int = _g12++;
					var rr : Number = Math.sqrt(1 - z * z);
					this.thumbs[k].pos = new V3(Math.cos(long) * rr * this.r,Math.sin(long) * rr * this.r,z * this.r);
					this.thumbs[k].z = z * this.r;
					z -= dz;
					long += s / rr;
				}
			}
			{
				var _g13 : int = 0, _g3 : int = this.n;
				while(_g13 < _g3) {
					var i2 : int = _g13++;
					this.thumbs[i2].display(this.thumbs[i2].pos,this.n,this.r);
				}
			}
			this.sortphotos();
		}
		
		protected function frame(evt : flash.events.Event) : void {
			this.framenum++;
			if(this.mouseonstage) {
				this.theta = -this.photos.mouseY * this.rotationSpeed / 1000.;
				this.phi = -this.photos.mouseX * this.rotationSpeed / 1000.;
			}
			else if(!this.waiting) {
				this.theta *= .9;
				this.phi *= .9;
			}
			var st : Number = Math.sin(this.theta);
			var ct : Number = Math.cos(this.theta);
			var sp : Number = Math.sin(this.phi);
			var cp : Number = Math.cos(this.phi);
			{
				var _g1 : int = 0, _g : int = this.n;
				while(_g1 < _g) {
					var i : int = _g1++;
					var x : Number = this.thumbs[i].pos.x;
					var y : Number = ct * this.thumbs[i].pos.y + st * this.thumbs[i].pos.z;
					var z : Number = -st * this.thumbs[i].pos.y + ct * this.thumbs[i].pos.z;
					var x2 : Number = cp * x + sp * z;
					var y2 : Number = y;
					var z2 : Number = -sp * x + cp * z;
					this.thumbs[i].pos = new V3(x2,y2,z2);
					this.thumbs[i].display(new V3(x2,y2,z2),this.n,this.r);
					this.thumbs[i].z = z2;
				}
			}
			if((this.phi != 0 || this.theta != 0) && this.framenum % 10 == 0) this.sortphotos();
		}
		
		protected function ns(_namespace : Namespace,name : String) : QName {
			return new QName(_namespace,new QName(name));
		}
		
		protected function sortphotos() : void {
			this.thumbs.sort(function(a : Thumb,b : Thumb) : int {
				return (a.z == b.z?0:(a.z > b.z?1:-1));
			});
			var i : int = this.n;
			while(--i >= 0) this.photos.setChildIndex(this.thumbs[i],i);
		}
		
		protected function shuffle(arr : Array) : void {
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
