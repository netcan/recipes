// https://www.codewars.com/collections/donaldsebleungs-roboscript
#[derive(Debug, Clone)]
struct RoboScript {
    dir: usize,
    cur_pos: (i32, i32),
    right_top: (i32, i32),
    left_down: (i32, i32),
    step: Vec<(i32, i32)>,
}

impl RoboScript {
    fn highlight(code: &str) -> String {
        let color = |c| {
            match c {
                'F'       => "pink",
                'L'       => "red",
                'R'       => "green",
                '0'..='9' => "orange",
                _         => unreachable!()
            }
        };

        let code = code.to_owned();
        let mut ans = String::new();
        let (mut i, mut j) = (0, 1);
        while j <= code.len() {
            if !(code.chars().nth(j).unwrap_or('X').is_ascii_digit() &&
                 code.chars().nth(i).unwrap().is_ascii_digit()) &&
                code.chars().nth(j) != code.chars().nth(i) {
                    if code.chars().nth(i) != Some('(') && code.chars().nth(i) != Some(')') {
                        ans += &format!("<span style=\"color: {}\">{}</span>",
                                        &color(code.chars().nth(i).unwrap()),
                                        &code[i..j]);
                    } else {
                        ans += &format!("{}", &code[i..j]);
                    }
                    i = j;
                }
            j += 1;
        }
        return ans;
    }

    const DIRECTION: [fn(&mut Self); 4] = [
        RoboScript::step_right,
        RoboScript::step_up,
        RoboScript::step_left,
        RoboScript::step_down
    ];

    fn step_right(&mut self) { self.cur_pos.0 += 1; }
    fn step_up(&mut self)    { self.cur_pos.1 += 1; }
    fn step_left(&mut self)  { self.cur_pos.0 -= 1; }
    fn step_down(&mut self)  { self.cur_pos.1 -= 1; }
    fn step_wrapper(&mut self, step: i32) {
        for _ in 0..step {
            (Self::DIRECTION[self.dir])(self);
            self.step.push(self.cur_pos);
            self.right_top = (self.cur_pos.0.max(self.right_top.0),
            self.cur_pos.1.max(self.right_top.1));
            self.left_down = (self.cur_pos.0.min(self.left_down.0),
            self.cur_pos.1.min(self.left_down.1));
        }
    }

    fn change_direction(&mut self, dir: u8, times: i32) {
        let times = times as usize;
        match dir {
            b'L' => self.dir = (self.dir + 4 + times) % 4,
            b'R' => self.dir = (self.dir + 3 * times) % 4,
            _ => unreachable!(),
        }
    }

    fn parse_num(code: &[u8], idx: usize) -> (i32, usize) {
        let (mut idy, mut step) = (idx, 0);
        while idy < code.len() && code[idy].is_ascii_digit() {
            step = step * 10 + (code[idy] - b'0') as i32;
            idy += 1;
        }
        if idy - idx == 0 { step = 1i32; }
        (step, idy - idx)
    }

    fn fetch_cmd(code: &[u8], idx: usize) -> (u8, i32, usize) {
        let cmd = code[idx];
        if cmd != b')' {
            let (step, len) = Self::parse_num(code, idx + 1);
            (cmd, step, len + 1)
        } else {
            (cmd, 1i32, 1)
        }
    }

    // may be (command) or command
    fn execute(&mut self, code: &[u8], idx: usize) -> usize {
        let mut idy = idx;
        while idy < code.len() {
            let (cmd, step, len) = Self::fetch_cmd(code, idy);
            idy += len; // eat (command and step)
            match cmd {
                b'(' => {
                    let grp_idx = idy;
                    let rollback_rb = self.clone();

                    idy += self.execute(code, idy); // run code
                    let (cnt, len) = Self::parse_num(code, idy);
                    idy += len; // eat 'num'
                    // cnt may be zero, rollback it
                    if cnt == 0 { *self = rollback_rb; }
                    for _ in 1..cnt {
                        self.execute(code, grp_idx);
                    }
                },
                b')' => return idy - idx, // backward
                b'L' | b'R' => self.change_direction(cmd, step),
                b'F' => self.step_wrapper(step),
                _ => unreachable!(),
            }
        }
        return idy - idx;
    }

    fn result(&mut self) -> String {
        let mut res = vec![vec![b' ';
        (self.right_top.0 - self.left_down.0 + 1) as usize];
        (self.right_top.1 - self.left_down.1 + 1) as usize];

        for (i, j) in &self.step {
            res[(self.right_top.1 - j) as usize]
                [(i- self.left_down.0) as usize] = b'*';
        }

        res.iter().map(|row|
                       std::str::from_utf8(row).unwrap()
        ).collect::<Vec<&str>>().join("\r\n")
    }

    fn run(code: &str) -> String {
        let mut rb = RoboScript {
            dir: 0,
            cur_pos: (0, 0),
            left_down: (0, 0),
            right_top: (0, 0),
            step: vec![(0, 0)],
        };
        rb.execute(code.as_bytes(), 0);
        rb.result()
    }
}

pub fn execute(code: &str) -> String {
    println!("code = {}", code);
    RoboScript::run(code)
}

pub fn highlight(code: &str) -> String {
    println!("code = {}", code);
    RoboScript::highlight(code)
}

fn main() {
    println!("{}", execute("FFLFFFLFFFFLFFFFFLFFFFFFLFFFFFFFLFFFFFFFFLFFFFFFFFFLFFFFFFFFFF"));
    println!("{}", execute("(L(F5(RF3))(((R(F3R)F7))))"));
    println!("{}", execute("F4L((F4R)2(F4L)2)2(F4R)2F4"));
    println!("{}", execute("((F2LF2R)2FRF4L(F2LF2R)2(FRFL)4(F2LF2R)2)2"));
}
