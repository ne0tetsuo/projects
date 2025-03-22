

(*NB: Si vous voulez tester si une formule est une tautologie, il faut l'inserer à la ligne 68*)
(* Types *) 
type prop = V of int | Vrai | Faux | Et of prop * prop | Ou of prop * prop | Imp of prop * prop | Equiv of prop * prop | Non of prop ;;
type cond = W of int | Vrai_bis | Faux_bis | Si of cond * cond * cond

                                                   
(* Transformation d'une formule prop en une formule cond *)
let rec trad (f: prop) : cond =
  match f with
  | V(i) -> W(i)
  | Vrai -> Vrai_bis
  | Faux -> Faux_bis
  | Et(p, q) -> Si(trad p, trad q, Faux_bis)
  | Ou(p, q) -> Si(trad p, Vrai_bis, trad q)
  | Imp(p, q) -> Si(trad p, trad q, Vrai_bis)
  | Equiv(p, q) -> Si(Si(trad p, trad q, Vrai_bis), Si(trad q, trad p, Vrai_bis), Vrai_bis)
  | Non(p) -> Si(trad p, Faux_bis, Vrai_bis);;


(* Mise en forme normale *)
let rec for_nor (g: cond) : cond =
  match g with
  | Si(Si(a, b, c), d, e) -> for_nor (Si(a, Si(b, d, e), Si(c, d, e)))
  | Si(a, b, c) -> Si(for_nor a, for_nor b, for_nor c)
  | _ -> g;;


(* Détermination si une formule est une tautologie *)
let rec eval (f: cond) (e: (int*bool)list) : bool =
  match f with
  | Vrai_bis -> true
  | Faux_bis -> false
  | W(i) ->
      (match List.assoc_opt i e with 
       | Some b -> b 
       | None -> false) 
  | Si(g, h, k) ->
      match g with
      | Vrai_bis -> eval h e 
      | Faux_bis -> eval k e 
      | W(i) ->
          (match List.assoc_opt i e with
           | Some b -> if b then eval h e else eval k e
           | None -> 
               let e1 = (i, true) :: e in 
               let e2 = (i, false) :: e in
               eval h e1 && eval k e2  
          )
      |Si(_,_,_)-> 
          failwith "Pattern non exhaustif: Si(_, _, _)";;
          
          

(* Fonction pour tester si une formule propositionnelle est une tautologie *)
let si_tautologie (f: prop) : bool =
  let g = trad f in
  print_string "formule traduite : %" 
    let h = for_nor g in
  eval h [] ;;

(* Exemple d'utilisation *)
let test () =

  let f = V(1) in
  let test = si_tautologie f in
  Printf.printf "La formule entrée est une tautologie : %b\n" test;; (*si c'est une tautologie ça affiche true sinon false *)
    
let f=V(1) in trad f;;
let


