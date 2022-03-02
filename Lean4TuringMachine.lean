import AxiomaticSystem

/-
NOTE: I am using List in place of sets

Axiomatic reconstruction of P versus NP problem as stated in the official problem outline provided by the Clay Institute
This outline can be found at 'https://www.claymath.org/sites/default/files/pvsnp.pdf'

Axiom naming convention: 
  * each axiom defining an object as a specific type is labeled as it appears in the problem outline
  * relations between these objects are of the form 'ax__[object 1]_[relation]_[object 2]_[relation]_..._[object n]'
-/

-- Formal def of Turing Machine
axiom Γ : List String -- finite alphabet including blank symbol 'b' that can be stored on the tape
def b : String := "" -- blank character
axiom ax__b_in_Γ : List.elem b Γ = true -- Prop that b is an element of Γ

axiom sigma : List String -- specified input alphabet of M, which is a subset of Γ and does not include b
axiom ax__sigma_subset_of_Γ : ∀ symbol : String, List.elem symbol sigma → List.elem symbol Γ -- for each symbol, if it is in sigma, it will be in Γ (ie. sigma ⊂ Γ)
axiom ax__not_b_in_sigma : List.elem b sigma = false -- the blank symbol b is not in sigma

axiom Q : List String -- set of possible states, stored in this formulation as strings
axiom q₀ : String -- the inital state of the machine
axiom q_accept : String -- special accept state
axiom q_reject : String -- special reject state
axiom ax__q₀_in_Q : List.elem q₀ Q = true -- the initial state is an element of Q
axiom ax__q_accept_in_Q : List.elem q_accept Q = true -- q_accept state is an element of Q
axiom ax__q_reject_in_Q : List.elem q_reject Q = true -- q_reject state is an element of Q

axiom transitions : (String × String) -- pair of strings representing the possible transitions of the machine head
axiom ax__plus_1_and_minus_1_in_transitions : transitions.1 = "-1" ∧ transitions.2 = "+1" -- axiom to encode the transitions as strings
axiom δ (q : String) (s : String) : (String × (String × String)) -- transition function of the form '(Q−{q_accept, q_reject}) × Γ → Q × Γ × {−1, 1}'
axiom ax__δ_returns_Q_Γ_transition : ∀ q s : String, 
  if List.elem q Q 
      /-
        q is an element of Q (ie. the current state of the machine)
      -/ 
    ∧ q != q_accept ∧ q != q_reject
      /-
        q is not q_accept or q_reject (ie. q ∈ Q-{q_accept, q_reject})
      -/
    ∧ List.elem s Γ
      /-
        s is an element of Γ (ie. s is the symbol on the current square)
      -/
  then
    List.elem (δ q s).1 Q = true
    /-
      the first member of the objects returned by δ, (Q × Γ × {−1, 1}), is an element of Q (ie. the new state of the machine)
    -/
    ∧ List.elem (δ q s).2.1 Γ = true
    /-
      the second member of the objects returned by δ, (Q × Γ × {−1, 1}), is an element of Γ (ie. the new symbol written to the current square)
    -/
    ∧ ((δ q s).2.2 = transitions.1 ∨ (δ q s).2.2 = transitions.2)
    /-
      the second member of the objects returned by δ, (Q × Γ × {−1, 1}), is '-1' or '1' (ie. the direction in which the head of the machine will move)
    -/
  else 
    (δ q s) = ("", "", "") -- if inputs are passed to the transition function that are outside of its domain, an empty output is returned

axiom T : ( -- axiom defining T to be a tuple of the form〈sigma, Γ, Q, δ〉
  List String /-sigma-/ 
  × List String /-Γ-/ 
  × List String /-Q-/ 
  × (String → String → String × String × String) /-δ-/
)
axiom ax__sigma_Γ_Q_δ_in_T : (T.1 = sigma) ∧ (T.2.1 = Γ) ∧ (T.2.2.1 = Q) ∧ (T.2.2.2 = δ) -- axiom specifying the elements of T
