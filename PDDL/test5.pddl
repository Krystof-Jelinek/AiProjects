(define (problem mapf1)
  (:domain mapf)
  (:requirements :strips)
  (:objects
   agent1, agent2, agent5
   t1 t2, t3, t4, t5, t6
   )
  (:init

    (adjacent t1 t4)
    (adjacent t4 t1)

    (adjacent t6 t4)
    (adjacent t4 t6)

    (adjacent t6 t5)
    (adjacent t5 t6)

    (adjacent t5 t2)
    (adjacent t2 t5)

    (adjacent t3 t5)
    (adjacent t5 t3)

    (free t2)
    (free t5)
    (free t6)

    (at agent2 t1)
    (at agent5 t4)
    (at agent1 t3)
    )
    (:goal
    (and
        (at agent1 t1)
        (at agent2 t2)
        (at agent5 t5)
        )
    )
)