mod parser;

use parser::scanner::scanner::Scanner;
use std::collections::HashSet;

fn main() {
    let file = String::from("a == 2;a>=2");

    let mut ops = HashSet::new();
    ops.insert(String::from("=="));
    ops.insert(String::from(">="));

    let chars: Vec<char> = file.chars().collect();

    let scanner = Scanner::new(&chars, &ops);

    for v in scanner {
        println!("{:?}", v)
    }
}