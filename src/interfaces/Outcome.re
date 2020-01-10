type t = {
  id: string,
  label: string,
  price: float,
};

external toOutcomeArray: 'a => array(t) = "%identity";