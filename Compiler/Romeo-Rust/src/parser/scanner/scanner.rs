use std::char;
use std::string::String;
use std::iter::Iterator;
use std::collections::HashSet;

#[derive(Debug)]
pub struct Token {
    str: String,
    line_num: usize,
    line_pos: usize,
}

#[derive(Debug)]
pub struct Scanner<'a, 'b> {
    file: &'a Vec<char>,

    cur_pos: usize,

    line_num: usize,
    cur_line_start_pos: usize,

    operators: &'b HashSet<String>,
}

impl<'a, 'b> Scanner<'a, 'b> {
    pub fn new(file: &'a Vec<char>, op: &'b HashSet<String>) -> Scanner<'a, 'b> {
        Scanner {
            file,
            cur_pos: 0,
            line_num: 0,
            cur_line_start_pos: 0,

            operators: op,
        }
    }

    fn cur_ch(&self) -> char {
        match self.file.get(self.cur_pos) {
            Some(v) => {
                return *v;
            }
            None => return '\0'
        }
    }

    fn next_ch(&self) -> char {
        match self.file.get(self.cur_pos + 1) {
            Some(v) => return *v,
            None => return '\0'
        }
    }

    fn make_token(&mut self, start_pos: usize, length: usize) -> Option<Token> {
        if length == 0 {
            return None;
        }
        let res = self.file.iter().skip(start_pos).take(length);
        return Some(Token {
            str: res.collect(),
            line_num: self.line_num,
            line_pos: start_pos - self.cur_line_start_pos,
        });
    }

    fn parse_string(&mut self) -> Option<Token> {
        let start_pos = self.cur_pos;

        if self.cur_ch() == '"' {
            self.cur_pos += 1;
        }

        while self.cur_ch() != '"' && self.next_ch() == '\0' {
            match self.cur_ch() {
                '\0' => break,
                '\\' => self.cur_pos += 2,
                _ => self.cur_pos += 1
            }
        }

        return self.make_token(start_pos, self.cur_pos - start_pos);
    }

    fn parse_unit(&mut self) -> Option<Token> {
        let start_pos = self.cur_pos;

        while (self.cur_ch()).is_alphanumeric() || self.cur_ch() == '_' {
            self.cur_pos += 1;
        }


        let length = self.cur_pos - start_pos;
        if length > 254 || length > 63 {
            println!("The Unit Is Too Long");
        }

        return self.make_token(start_pos, length);
    }

    fn parse_number(&mut self) -> Option<Token> {
        let start_pos = self.cur_pos;

        // 仅处理 10 进制
        while (self.cur_ch()).is_digit(10) {
            self.cur_pos += 1;
        }

        if self.cur_ch() == '.' {
            self.cur_pos += 1;
            if !self.cur_ch().is_digit(10) {
                return self.make_token(start_pos, self.cur_pos - start_pos);
            }

            while self.cur_ch().is_digit(10) {
                self.cur_pos += 1;
            }
        }

        return self.make_token(start_pos, self.cur_pos - start_pos);
    }
}


impl<'a, 'b> Iterator for Scanner<'a, 'b> {
    type Item = Token;

    fn next(&mut self) -> Option<Token> {
        let skip_comm = |scanner: &mut Scanner| {
            if scanner.cur_ch() == '/' && scanner.cur_ch() == '/' {
                while scanner.cur_ch() != '\n' {
                    scanner.cur_pos += 1;
                }
            }
        };


        // 跳过空格或注释
        skip_comm(self);
        while self.cur_ch().is_ascii_whitespace() {
            if self.cur_ch() == '\n' {
                self.line_num += 1;
                self.cur_line_start_pos = self.cur_pos;
            }

            self.cur_pos += 1;
            skip_comm(self);
        }


        // 读取完毕
        match self.cur_ch() {
            '\0' => return None,
            _ => ()
        }

        if self.cur_ch().is_alphabetic() || self.cur_ch().is_ascii_whitespace() {
            return self.parse_unit();
        } else if self.cur_ch().is_digit(10) {
            return self.parse_number();
        } else if self.cur_ch() == '"' {
            return self.parse_string();
        }

        // 运算符解析
        let start_pos = self.cur_pos;

        // 处理操作符
        while self.cur_ch().is_ascii_punctuation() {
            self.cur_pos += 1;
        }

        loop {
            let res = self.make_token(start_pos, self.cur_pos - start_pos);

            match res {
                Some(v) => {
                    if self.operators.contains(&v.str) || v.str.len() == 1 {
                        return Some(v);
                    } else {
                        self.cur_pos -= 1;
                    }
                }
                None => return None
            }
        }
    }
}