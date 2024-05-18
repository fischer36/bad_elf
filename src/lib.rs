// #[link(name = "init", kind = "dylib")]

extern "C" {
    fn read_mappings() -> i32;
}

#[no_mangle]
pub extern "C" fn rust_function() {
    println!("rust_function");
}

#[no_mangle]
pub extern "C" fn rusty_entry() {
    eprintln!("rusty_entry{}", 1 + 2); // Using eprintln! to print to stderr
    unsafe {
        read_mappings();
    }
    check_libs();
    std::thread::sleep(std::time::Duration::from_secs(1));
}

use libc::{c_int, dl_phdr_info};
use std::ffi::CStr;
mod parse;

static mut COUNT: u32 = 0;
// return value of zero continues the iteration and a non-zero breaks it.
extern "C" fn callback(info: *mut dl_phdr_info, _size: usize, _data: *mut libc::c_void) -> c_int {
    unsafe {
        println!("count {}", COUNT);
        if COUNT > 1 {
            return -1;
        }
        COUNT += 1;
        let info_ref = &*info;
        if !info_ref.dlpi_name.is_null() {
            let c_str = CStr::from_ptr(info_ref.dlpi_name);
            let name = c_str.to_str().unwrap_or("");

            println!("Loaded library name: {}, Segment count: {}", name, info_ref.dlpi_phnum,);

            for i in 0..info_ref.dlpi_phnum {
                let phdr_ptr = info_ref.dlpi_phdr.add(i as usize);
                let phdr = *phdr_ptr;

                let segment_start = info_ref.dlpi_addr + phdr.p_vaddr;
                let segment_end = segment_start + phdr.p_memsz;

                // println!("Start {:#x} End {:#x} Size {:#x}", segment_start, segment_end, segment_end - segment_start);

                if phdr.p_type == goblin::elf::program_header::PT_DYNAMIC {
                    let dyn_seg_addr = (*info_ref).dlpi_addr + phdr.p_vaddr;
                    println!("\nDynamic Segment: {:#x}", dyn_seg_addr);
                    parse_dynamic_section(dyn_seg_addr as *const goblin::elf::dynamic::Dyn);
                    return -1;
                }
            }
        }

        0
    }
}

fn check_libs() {
    println!("check_libs - Checking for libraries...");
    unsafe {
        libc::dl_iterate_phdr(Some(callback), std::ptr::null_mut());
    }
}

extern "C" fn printf_hook(format: *const libc::c_char, val: i32) {
    println!("my_function called");
    unsafe {
        let c_str = CStr::from_ptr(format);
        match c_str.to_str() {
            Ok(str_slice) => println!("{}", str_slice),
            Err(e) => eprintln!("Failed to convert C string to Rust string: {}", e),
        }
    }
}

unsafe fn parse_dynamic_section(dyn_ptr: *const goblin::elf::dynamic::Dyn) {
    let mut section_count = 0;
    while (*dyn_ptr.add(section_count)).d_tag != goblin::elf::dynamic::DT_NULL {
        pub const DT_PLTGOT: u64 = 3;
        let tag = (*dyn_ptr.add(section_count)).d_tag;

        if tag == DT_PLTGOT {
            let mut count = 0;

            let ptr = ((*dyn_ptr.add(section_count)).d_val as *mut u64).add(3); // Store the address as usize
            while count < 13 {
                let entry = ptr.add(count);
                if *entry == 0 {
                    break;
                }
                println!("GOT entry: {} address: {:#x?}", count, *entry);
                count += 1;
            }
            println!("GOT count : {:?} ", count);
            break;
        }
        section_count += 1;
    }
}
