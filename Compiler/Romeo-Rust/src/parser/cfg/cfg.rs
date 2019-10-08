use std::collections::HashSet;
use std::io::Read;

use std::fmt;
use std::str::FromStr;
use nom::IResult;
use parsers;
use error::Error;

/// A Term can represent a Terminal or Nonterminal node
#[derive(PartialEq, Debug, Clone)]
pub enum Term {
    Terminal(String),
    Nonterminal(String),
}

impl Term {
    // Get `Term` by parsing a string
    pub fn from_str(s: &str) -> Result<Self, Error> {
        match parsers::term_complete(s.as_bytes()) {
            IResult::Done(_, o) => Ok(o),
            IResult::Incomplete(n) => Err(Error::from(n)),
            IResult::Error(e) => Err(Error::from(e)),
        }
    }
}

impl FromStr for Term {
    type Err = Error;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        Self::from_str(s)
    }
}

impl fmt::Display for Term {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            Term::Terminal(ref s) => {
                if s.contains('"') {
                    write!(f, "'{}'", s)
                } else {
                    write!(f, "\"{}\"", s)
                }
            }
            Term::Nonterminal(ref s) => write!(f, "<{}>", s),
        }
    }
}