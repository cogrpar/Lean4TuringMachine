-- ABI functions

@[extern "general_transition_function"] constant GeneralTransitionFunction
  /- Q, Γ, accept, reject, and transitions passed from an instance of a TM -/
  (Q : List String) (Γ : List String) (q_accept q_reject : String) (transitions : (String × String))
  /- Machine program as a yaml table stored in a string -/
  (program : String)
  /- transition function of the form '(Q−{q_accept, q_reject}) × Γ → Q × Γ × {−1, 1}' -/
  (q s : String) (Q_len : Float := Q.length.toFloat) (Γ_len : Float := Γ.length.toFloat) : (String × String × String)

-- this function returns a true or false value depending on if the passed element String is a member of the passed List String
@[extern "check_mem"] def CheckMem (e : String) (l : List String) (l_len : Float := l.length.toFloat) : Bool := true
