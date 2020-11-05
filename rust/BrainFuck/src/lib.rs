/*************************************************************************
    > File Name: lib.rs
    > Author: Netcan
    > Descripton: BrainFuck compiler
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-11-05 23:32
************************************************************************/
extern crate proc_macro;
use std::str::FromStr;
use std::str::from_utf8;
use proc_macro::TokenStream;

const CELL_SIZE: usize = 16;
fn parse(code: &str, cells: &mut [u8; CELL_SIZE], pc: &mut usize, output: &mut Vec<u8>) {
    let mut idx = 0;
    while idx < code.len() {
        let c = code.as_bytes()[idx];
        match c {
            b'+' => cells[*pc] += 1,
            b'-' => cells[*pc] -= 1,
            b'>' => *pc += 1,
            b'<' => *pc -= 1,
            b'[' => {
                while cells[*pc] != 0 {
                    parse(&code[idx+1..], cells, pc, output);
                }
                idx += code[idx..].find(']').unwrap();
            },
            b'.' => output.push(cells[*pc]),
            b']' => return,
            _ => {}
        }
        idx += 1;
    }
}

#[proc_macro]
pub fn brain_fuck(_item: TokenStream) -> TokenStream {
    let input = _item.to_string();
    let mut cells: [u8; CELL_SIZE] = [0; CELL_SIZE];
    let mut pc = 0;
    let mut output = Vec::<u8>::new();

    parse(&input, &mut cells, &mut pc, &mut output);

    TokenStream::from_str(
        &format!("\"{}\"", from_utf8(&output).unwrap())
    ).unwrap()
}
