-- ABI functions
@[extern "general_transition_function"] constant GeneralTransitionFunction
  /- Q, Γ, and transitions passed from an instance of a TM -/
  (Q Γ : List String) (transitions : (String × String))
  /- Machine program as a yaml table stored in a string -/
  (program : String)
  /- transition function of the form '(Q−{q_accept, q_reject}) × Γ → Q × Γ × {−1, 1}' -/
  (q s : String) : (String × String × String)

