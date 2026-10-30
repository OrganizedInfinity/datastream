{
  description = "libdatastream";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      lib = pkgs.lib;
    in
    rec {
      packages.${system}.default = lib.customisation.callPackageWith pkgs ./default.nix {};
      devShells.${system}.default = pkgs.mkShell {
        inputsFrom = [ packages.${system}.default ];
        packages = with pkgs; [ clang-tools nixd ];
        mesonBuildType = "debug";
      };
    };
}
