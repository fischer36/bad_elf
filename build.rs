fn main() {
    println!("cargo:rerun-if-changed=src/init.c");
    cc::Build::new().file("src/init.c").compile("init");
    // println!("cargo:rustc-link-search=/home/thinkerpad/Desktop/hahaXd/build");
    //
    // println!("cargo:rustc-link-lib=dylib=init");
}
