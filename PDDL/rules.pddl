(define (domain mapf)
 (:requirements :strips)
 
 (:predicates
   (at ?a ?t)
   (free ?t)
   (adjacent ?t1  ?t2)
 )

 (:action move
        :parameters (?agent ?from ?to)
        :precondition (and (adjacent ?from ?to)
                            (free ?to)
                            (at ?agent ?from)
                            )
        :effect (and (at ?agent ?to)
                     (free ?from)
                     (not (free ?to))
                     (not (at ?agent ?from))
                     )
 )
)