{
  lib,
  meson,
  ninja,
  pkg-config,
  stdenv
}: stdenv.mkDerivation {
  pname = "datastream";
  version = lib.strings.trim (builtins.readFile ./version.txt);
  src = ./.;
  nativeBuildInputs = [ meson ninja pkg-config ];
  mesonBuildType = "release";
  doCheck = true;
  meta = {
    description = "A stream parsing library";
    license = lib.licenses.gpl2Plus;
    platforms = lib.platforms.linux;
  };
}
