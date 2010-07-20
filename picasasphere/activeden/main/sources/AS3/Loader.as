package  {
	import flash.utils.Timer;
	import flash.events.TimerEvent;
	import flash.display.Sprite;
	import flash.Boot;
	public class Loader extends flash.display.Sprite {
		public function Loader(color1 : int = -1,color2 : int = -1) : void { if( !flash.Boot.skip_constructor ) {
			super();
			this.active = 0;
			this.color1 = (color1 < 0?8930304:color1);
			this.color2 = (color2 < 0?8947848:color2);
			this.numdots = 3;
			this.separation = 16;
			this.milliseconds = 800;
			this.timer = new flash.utils.Timer(this.milliseconds,0);
			this.timer.addEventListener(flash.events.TimerEvent.TIMER,this.render);
			this.timer.start();
			this.render(null);
		}}
		
		protected var active : int;
		protected var color1 : int;
		protected var color2 : int;
		protected var numdots : int;
		protected var milliseconds : int;
		protected var separation : int;
		protected var timer : flash.utils.Timer;
		protected function render(evt : flash.events.TimerEvent) : void {
			this.active = (this.active + 1) % this.numdots;
			this.graphics.clear();
			{
				var _g1 : int = 0, _g : int = this.numdots;
				while(_g1 < _g) {
					var i : int = _g1++;
					this.graphics.beginFill((this.active == i?this.color1:this.color2));
					this.graphics.drawCircle((i - this.numdots / 2.) * this.separation,0,3);
					this.graphics.endFill();
				}
			}
		}
		
	}
}
