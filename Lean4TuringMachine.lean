import AxiomaticSystem

namespace List
  -- define a function that removes all instances of a passed element from a list (corresponds to setminus when using lists in place of sets)
  def remove {α : Type u} [BEq α] : α → List α → List α
    | element, nil => nil
    | element, cons first rest => 
      if first == element then
        remove element rest
      else
        [first] ++ (remove element rest)

  -- define a function that checks to see if all elements of one list are elements of another list (corresponds to A ⊂ B when using lists in place of sets)
  def sublist {α : Type u} [BEq α] : List α → List α → Bool
    | nil, nil             => true
    | nil, cons first rest => true
    | cons first rest, nil => false
    | cons firstA restA, cons firstB restB =>
      if firstA == firstB ∨ List.elem firstA restB then -- if the first element of A is in B
        sublist restA ([firstB]++restB) -- check the rest of A
      else false
end List

/-
NOTE: I am using List in place of sets

Axiomatic reconstruction of Turing Machine from P versus NP problem as stated in the official problem outline provided by the Clay Institute
This outline can be found at 'https://www.claymath.org/sites/default/files/pvsnp.pdf'

Axiom naming convention: 
  * each axiom defining an object as a specific type is labeled as it appears in the problem outline
  * relations between these objects are of the form 'ax__[object 1]_[relation]_[object 2]_[relation]_..._[object n]'
-/

-- Formal def of Turing Machine
namespace TuringMachine
  -- Define the blank symbol
  constant b : String := " "

  -- Define the finite alphabet
  constant Γ : List String := ["1", "0", b] -- finite alphabet including blank symbol 'b' that can be stored on the tape

  -- Define the machine's input alphabet
  constant sigma : List String := List.remove b Γ -- specified input alphabet of M, which is a subset of Γ and does not include b

  -- Define the machine's states
  constant Q : List String := ["right", "carry", "done"] -- list of possible states the machine can be in
  constant q₀ : String := List.get! 0 Q -- state that the machine starts in
  constant q_accept : String := List.get! 2 Q -- state that results in the machine halting once reached, signifying the program ran to completion successfully
  constant q_reject : String := "reject" -- state that results in the machine halting once reached, signifying the transition function rejected its inputs

  -- Define the transition function
  constant transitions : (String × String) := ("-1", "+1") -- pair of strings representing the possible transitions of the machine head
  constant δ (q : String) (s : String) : (String × (String × String)) := -- transition function of the form '(Q−{q_accept, q_reject}) × Γ → Q × Γ × {−1, 1}'
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
    if (q == Q.get! 0) then
      if (List.elem s [Γ.get! 0, Γ.get! 1]) then
        (Q.get! 0, s, transitions.2) -- move right
      else
        (Q.get! 1, s, transitions.1) -- move left
    else if (q == Q.get! 1) then
      if (List.elem s [Γ.get! 0]) then
        (Q.get! 1, Γ.get! 1, transitions.1) -- move left
      else
        (Q.get! 2, Γ.get! 0, transitions.1) -- move left
    else -- done
      (q_accept, s, transitions.2) -- accept state, don't change tape, move to the right once more before halting
  else -- transition function rejects the inputs
    (q_reject, s, transitions.2) -- reject state, don't change tape, move to the right once more before halting

end TuringMachine

-- Define the componenets needed to impliment a machine, namely the tape
constant Tape : List String := ["1", "0", "1", " ", " "]


-- Prove that the instance of the machine defined above is valid

