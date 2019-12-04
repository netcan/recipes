use std::collections::BinaryHeap;
use std::rc::Rc;
use std::cmp::{Ord, Ordering};

#[derive(Debug, PartialEq)]
enum NodeType {
    Start,
    End,
    Block,
    Empty,
}

impl From<&str> for NodeType {
    fn from(c: &str) -> Self {
        use NodeType::*;
        match c {
            "" | "*" => Empty,
            "S"      => Start,
            "E"      => End,
            "B"      => Block,
            _        => unreachable!()
        }
    }
}

#[derive(PartialEq, Eq)]
struct State {
    pos: (usize, usize),
    cost: usize,
    last_status: Option<Rc<State>>
}

impl Ord for State {
    fn cmp(&self, other: &Self) -> Ordering {
        self.cost.partial_cmp(&other.cost).unwrap()
    }
}

impl PartialOrd for State {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        // return Some(self.cost.cmp(&other.cost));
        Some(Ordering::Equal)
    }
}

fn find_shortest_path(map: &[Vec<NodeType>]) -> Option<Rc<State>> {
    const DX: [i32; 8] = [0, 1, 0, -1, 1, 1, -1, -1];
    const DY: [i32; 8] = [1, 0, -1, 0, 1, -1, -1, 1];
    let (height, width) = (map.len(), map[0].len());

    let mut visited = vec![vec![false; width]; height];

    let mut que = BinaryHeap::new();
    let start_pos = (0, 0);
    visited[start_pos.0][start_pos.1] = true;
    que.push(Rc::new(State {
        pos: start_pos,
        cost: 0,
        last_status: None
    }));

    while let Some(s) = que.pop() {
        let (y, x) = s.pos;
        if map[y][x] == NodeType::End {
            return Some(s);
        }
        for d in 0..8 {
            let (ny, nx) = (y as i32 + DY[d], x as i32 + DX[d]);
            if ny >= 0 && ny < height as i32 &&
                nx >= 0 && nx < width as i32 {
                let (ny, nx) = (ny as usize, nx as usize);
                if !visited[ny][nx] && map[ny][nx] != NodeType::Block {
                    visited[ny][nx] = true;
                    que.push(Rc::new(State {
                        pos: (ny, nx),
                        cost: 0,
                        last_status: Some(Rc::clone(&s))
                    }));
                }
            }
        }
    }

    None
}

fn show_path(path: &Option<Rc<State>>) {
    match path {
        Some(s) => {
            show_path(&s.last_status);
            println!("{:?}", s.pos);
        },
        None => {}
    }
}

fn main() {
    let map = vec![
        vec![NodeType::Start, NodeType::Empty, NodeType::Empty, NodeType::Empty],
        vec![NodeType::Empty, NodeType::Empty, NodeType::Empty, NodeType::End],
    ];
    show_path(&find_shortest_path(&map));
}
