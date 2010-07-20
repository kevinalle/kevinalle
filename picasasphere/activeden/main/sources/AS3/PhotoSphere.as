package  {
	import flash.Lib;
	public class PhotoSphere {
		static protected var params : *;
		static public function main() : void {
			PhotoSphere.params = flash.Lib.current.loaderInfo.parameters;
			flash.Lib.current.addChild(new Sphere(params.service,params.user,params.album,Std._parseInt(params.thumbnailquality),params.xml,Std._parseInt(params.max)));
		}
		
	}
}
