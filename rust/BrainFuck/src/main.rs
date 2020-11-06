/*************************************************************************
    > File Name: main.rs
    > Author: Netcan
    > Descripton: BrainFuck compiler
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-11-05 23:32
************************************************************************/
extern crate brain_fuck;
use brain_fuck::brain_fuck;

fn main() {
    println!("{}", brain_fuck!(
        ++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.
        >---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.
    ));
}

