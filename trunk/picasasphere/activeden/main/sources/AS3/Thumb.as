package  {
	import flash.display.Loader;
	import flash.net.URLRequest;
	import flash.events.MouseEvent;
	import flash.Lib;
	import flash.filters.DropShadowFilter;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.display.DisplayObject;
	import flash.Boot;
	public class Thumb extends flash.display.Sprite {
		public function Thumb(url : String = null,link : String = null) : void { if( !flash.Boot.skip_constructor ) {
			super();
			var ldr : flash.display.Loader = new flash.display.Loader();
			ldr.name = "loader";
			ldr.load(new flash.net.URLRequest(url));
			ldr.contentLoaderInfo.addEventListener(flash.events.Event.COMPLETE,this.loaded);
			this.rotation = (Math.random() - .5) * 20;
			var drpShdw : flash.filters.DropShadowFilter = new flash.filters.DropShadowFilter(2,45,0,1,8,8,.6,1);
			this.filters = [drpShdw];
			this.addChild(ldr);
			this.link = link;
			if(this.link != null) this.addEventListener(flash.events.MouseEvent.CLICK,this.openurl);
			this.addEventListener(flash.events.MouseEvent.MOUSE_OVER,this.over);
		}}
		
		public var link : String;
		public var pos : V3;
		public var z : Number;
		protected function loaded(evt : flash.events.Event = null) : void {
			var ldr : flash.display.DisplayObject = this.getChildByName("loader");
			var w : Number = ldr.width;
			var h : Number = ldr.height;
			if(w > h) {
				h = h * 72 / w;
				w = 72;
			}
			else {
				w = w * 72 / h;
				h = 72;
			}
			ldr.width = w;
			ldr.height = h;
			var sze : Number = (ldr.width > ldr.height?ldr.width:ldr.height);
			this.buttonMode = true;
			this.graphics.beginFill(16777215);
			this.graphics.drawRect(-sze / 2 - 4,-sze / 2 - 4,sze + 8,sze + 8);
			this.mouseChildren = false;
			ldr.x = -ldr.width / 2;
			ldr.y = -ldr.height / 2;
		}
		
		public function display(pos : V3,n : int,r : Number) : void {
			this.z = pos.z;
			this.x = pos.x;
			this.y = pos.y;
			this.alpha = pos.z / (3. * r) + .66;
			this.scaleX = this.scaleY = (4.5 / Math.sqrt(n)) * (.8 * pos.z / (2. * r) + .7) * (4. / 3);
		}
		
		protected function over(evt : flash.events.MouseEvent) : void {
			var rect : flash.display.Sprite = new flash.display.Sprite();
			rect.graphics.beginFill(0,0);
			rect.graphics.drawRect(-1.2 * this.width / 2,-1.2 * this.height / 2,1.2 * this.width,1.2 * this.height);
			rect.name = "rect";
			this.addChild(rect);
			this.pos.scaleBy(1.1);
			this.addEventListener(flash.events.MouseEvent.MOUSE_OUT,this.out);
		}
		
		protected function out(evt : flash.events.MouseEvent) : void {
			this.removeChild(this.getChildByName("rect"));
			this.removeEventListener(flash.events.MouseEvent.MOUSE_OUT,this.out);
			this.pos.scaleBy(1 / 1.1);
		}
		
		protected function openurl(evt : flash.events.MouseEvent) : void {
			var url : flash.net.URLRequest = new flash.net.URLRequest(this.link);
			flash.Lib.getURL(url,"_blank");
		}
		
		public function normalize(r : Number) : void {
			this.pos.normalize(r);
		}
		
	}
}
