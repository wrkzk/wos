with import <nixpkgs> {
  crossSystem = {
    config = "i686-elf";
  };
};

mkShell {
  buildInputs = [];
}