(define (problem mapf1)
  (:domain mapf)
  (:requirements :strips)
  (:objects
   agent1, agent2, agent3
   t1 t2, t3, t4
   )
  (:init
    (adjacent t1 t2)
    (adjacent t2 t1)

    (adjacent t2 t3)
    (adjacent t3 t2)

    (adjacent t1 t4)
    (adjacent t4 t1)

    (adjacent t2 t4)
    (adjacent t4 t2)

    (adjacent t3 t4)
    (adjacent t4 t3)
    
    (free t4)

    (at agent1 t3)
    (at agent2 t1)
    (at agent3 t2)
    )
    (:goal
    (and
        (at agent1 t1)
        (at agent2 t2)
        (at agent3 t3)
        )
    )
)