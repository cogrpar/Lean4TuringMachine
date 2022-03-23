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
  axiom ax__b_in_Γ : (List.elem b Γ)=true -- postulate that the provided Γ contains b (TODO have the proofs of the conditions come from c++ functions using the lean ABI)

  -- Define the machine's input alphabet
  constant sigma : List String := List.remove b Γ -- specified input alphabet of M, which is a subset of Γ and does not include b
  axiom ax__sigma_subset_Γ : (List.sublist sigma Γ)=true -- postulate that the provided sigma is a subset of Γ
  axiom ax__b_not_in_sigma : (List.elem b sigma)=false -- postulate that the provided sigma does not contain b

  -- Define the machine's states
  constant Q : List String := ["right", "carry", "done"] -- list of possible states the machine can be in
  constant q₀ : String := List.get! 0 Q -- state that the machine starts in
  constant q_accept : String := List.get! 2 Q -- state that results in the machine halting once reached, signifying the program ran to completion successfully
  constant q_reject : String := "reject" -- state that results in the machine halting once reached, signifying the transition function rejected its inputs
  axiom ax__q₀_q_accept_q_reject_in_Q : (List.elem q₀ Q)=true ∧ (List.elem q_accept Q)=true ∧ (List.elem q_reject Q)=true -- postulate that the provided Q contains q₀, q_accept, and q_reject

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
  axiom ax__δ_inputs_yield_correct_outputs : ∀(q s : String),
    if ¬(List.elem q (List.remove q_accept (List.remove q_reject Q)) ∧ List.elem s Γ) then 
      (δ q s).1 = q_reject
    else
      ¬((δ q s).1 = q_reject)
  axiom ax__δ_outputs_are_correct_form : ∀(q s : String),
      List.elem (δ q s).1 Q 
      ∧ List.elem (δ q s).2.1 Γ
      ∧ ((δ q s).2.2 = transitions.1 ∨ (δ q s).2.2 = transitions.2)


  -- Instance of Turing Machine
  def TM := (sigma, Γ, Q, δ)

  -- Postulate the conditions for an object with the structure of a turing machine to be a valid machine
  constant valid : (List String × List String × List String × (String → String → String × String × String)) → Bool -- maps an object with the structure of a turing machine to its validity
  axiom T : 
    ∀ t : ( -- structure of T is a tuple of the form〈sigma, Γ, Q, δ〉
      List String /-sigma-/ 
      × List String /-Γ-/ 
      × List String /-Q-/ 
      × (String → String → String × String × String) /-δ-/
    ),
    let σ := t.1
    let γ := t.2.1
    let ϙ := t.2.2.1
    let Δ := t.2.2.2
      
    -- condition 1: Γ contains b
    (List.elem b γ) = true →
    -- condition 2: sigma is a subset of Γ
    (List.sublist σ γ) = true →
    -- condition 3: sigma does not contain b
    (List.elem b σ) = false →
    -- condition 4: Q contains q₀, q_accept, and q_reject
    (List.elem q₀ ϙ) = true ∧ (List.elem q_accept ϙ) = true ∧ (List.elem q_reject ϙ) = true →
    -- condition 5: structure of δ
    -- a. the first input is an element of Q and not q_accept or q_reject, and the second input is an element of Γ (if this is not the case, q_reject is returned)
    ((q s : String) → 
    if ¬(List.elem q (List.remove q_accept (List.remove q_reject ϙ)) ∧ List.elem s γ) then 
      (Δ q s).1 = q_reject
    else
      ¬((Δ q s).1 = q_reject)) →
    -- b. the output is of the form Q × Γ × {−1, 1}
    ((q s : String) →
    List.elem (Δ q s).1 ϙ
    ∧ List.elem (Δ q s).2.1 γ
    ∧ ((Δ q s).2.2 = transitions.1 ∨ (Δ q s).2.2 = transitions.2))

    → valid t = true
end TuringMachine


-- prove that the above instance of the machine is valid
theorem showValid : (TuringMachine.valid TuringMachine.TM = true) :=
    show TuringMachine.valid TuringMachine.TM = true from 
      TuringMachine.T TuringMachine.TM
        TuringMachine.ax__b_in_Γ -- condition 1
        TuringMachine.ax__sigma_subset_Γ -- condition 2
        TuringMachine.ax__b_not_in_sigma -- condition 3
        TuringMachine.ax__q₀_q_accept_q_reject_in_Q -- condition 4
        TuringMachine.ax__δ_inputs_yield_correct_outputs -- condition 5a
        TuringMachine.ax__δ_outputs_are_correct_form -- condition 5b

#check showValid

-- Define the componenets needed to impliment a machine, namely the tape, the current position of the machine head on the tape (measured as number of spaces from the leftmost cell), and the current state
constant Tape : (List String × Nat × String) := (["1", "0", "1"], 0, "right")

-- function that takes an instance of a Turing Machine, a tape of symbols, and the current position of the machine head and returns the updated tape and head position
def stepMachine : (List String × List String × List String × (String → String → String × String × String)) → (List String × Nat × String) → (List String × Nat × String) :=
  fun tm : (List String × List String × List String × (String → String → String × String × String)) =>
    let σ := tm.1
    let γ := tm.2.1
    let ϙ := tm.2.2.1
    let Δ := tm.2.2.2
  
    fun tape : (List String × Nat × String) => 
      let tape_data := tape.1
      let tape_pos := tape.2.1
      let tape_data_current := 
        if tape_pos >= (List.length tape_data) then -- blank symbol as this cell has not been written to yet
          TuringMachine.b
        else 
          tape_data.get! tape_pos

      let machine_state := tape.2.2

      -- make sure that the current symbols on the tape is within the input library of tm (γ)
      if ¬(List.elem tape_data_current γ) then 
        -- this symbol is not within the set of valid input symbols, so return the current tape and position, and set the state to q_reject
        (tape_data, tape_pos, TuringMachine.q_reject)
      else -- the current symbol is valid
        (tape_data, tape_pos, TuringMachine.q_accept) -- TODO finish this

#eval stepMachine TuringMachine.TM Tape