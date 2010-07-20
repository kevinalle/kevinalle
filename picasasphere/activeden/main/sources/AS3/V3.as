package  {
	import flash.Boot;
	public class V3 {
		public function V3(x : Number = NaN,y : Number = NaN,z : Number = NaN) : void { if( !flash.Boot.skip_constructor ) {
			this.x = x;
			this.y = y;
			this.z = z;
		}}
		
		public var x : Number;
		public var y : Number;
		public var z : Number;
		public function normalize(r : Number = 1) : void {
			var norm : Number = Math.sqrt(this.x * this.x + this.y * this.y + this.z * this.z);
			this.x *= r / norm;
			this.y *= r / norm;
			this.z *= r / norm;
		}
		
		public function normalized(r : Number = 1) : V3 {
			var norm : Number = Math.sqrt(this.x * this.x + this.y * this.y + this.z * this.z);
			return new V3(this.x * r / norm,this.y * r / norm,this.z * r / norm);
		}
		
		public function add(v : V3) : V3 {
			return new V3(this.x + v.x,this.y + v.y,this.z + v.z);
		}
		
		public function subtract(v : V3) : V3 {
			return new V3(this.x - v.x,this.y - v.y,this.z - v.z);
		}
		
		public function scaleBy(s : Number) : void {
			this.x *= s;
			this.y *= s;
			this.z *= s;
		}
		
		public function scaledBy(s : Number) : V3 {
			return new V3(this.x * s,this.y * s,this.z * s);
		}
		
		public function norm() : Number {
			return Math.sqrt(this.x * this.x + this.y * this.y + this.z * this.z);
		}
		
		public function crossp(b : V3) : V3 {
			return new V3(this.y * b.z - this.z * b.y,this.z * b.x - this.x * b.z,this.x * b.y - this.y * b.x);
		}
		
		public function dotp(b : V3) : Number {
			return this.x * b.x + this.y * b.y + this.z * b.z;
		}
		
		public function print() : String {
			return Math.round(this.x * 100) / 100. + " " + Math.round(this.y * 100) / 100. + " " + Math.round(this.z * 100) / 100.;
		}
		
	}
}
