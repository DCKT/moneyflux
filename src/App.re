open Belt;

module LocalStorage = {
  [@bs.val]
  external getItem: string => Js.Nullable.t(string) = "localStorage.getItem";
  [@bs.val]
  external setItem: (string, string) => unit = "localStorage.setItem";
};

type state = {
  income: float,
  outcome: array(Outcome.t),
  isCreating: bool,
};

type action =
  | SetIncome(float)
  | AddOutcome(Outcome.t)
  | UpdateOutcome(Outcome.t)
  | DeleteOutcome(Outcome.t)
  | SaveOutcomeToStorage
  | ToggleCreationForm
  | OnCreationSubmit(Outcome.t);

[@react.component]
let make = () => {
  let initialState = {
    income:
      LocalStorage.getItem("income")
      ->Js.Nullable.toOption
      ->Option.map(value => value->Js.Float.fromString)
      ->Option.getWithDefault(0.),
    outcome:
      LocalStorage.getItem("outcome")
      ->Js.Nullable.toOption
      ->Option.map(value => {value->Js.Json.parseExn->Outcome.toOutcomeArray})
      ->Option.getWithDefault([||]),
    isCreating: false,
  };

  let (state, send) =
    ReactUpdate.useReducer(initialState, (action, state) =>
      switch (action) {
      | ToggleCreationForm =>
        Update({...state, isCreating: !state.isCreating})
      | OnCreationSubmit(outcome) =>
        UpdateWithSideEffects(
          {...state, isCreating: false},
          self => {
            self.send(AddOutcome(outcome));
            None;
          },
        )
      | SetIncome(income) =>
        UpdateWithSideEffects(
          {...state, income},
          self => {
            LocalStorage.setItem(
              "income",
              self.state.income->Js.Float.toString,
            );

            None;
          },
        )
      | AddOutcome(outcome) =>
        UpdateWithSideEffects(
          {...state, outcome: state.outcome->Array.concat([|outcome|])},
          self => {
            self.send(SaveOutcomeToStorage);

            None;
          },
        )

      | UpdateOutcome(outcome) =>
        UpdateWithSideEffects(
          {
            ...state,
            outcome:
              state.outcome->Array.map(o => o.id === outcome.id ? outcome : o),
          },
          self => {
            self.send(SaveOutcomeToStorage);
            None;
          },
        )
      | DeleteOutcome(outcome) =>
        UpdateWithSideEffects(
          {
            ...state,
            outcome: state.outcome->Array.keep(o => o.id !== outcome.id),
          },
          self => {
            self.send(SaveOutcomeToStorage);
            None;
          },
        )
      | SaveOutcomeToStorage =>
        SideEffects(
          self => {
            self.state.outcome
            ->Js.Json.stringifyAny
            ->Option.map(value => {LocalStorage.setItem("outcome", value)})
            ->ignore;
            None;
          },
        )
      }
    );

  let remaining =
    state.income
    -. state.outcome->Array.reduce(0., (sum, outcome) => outcome.price +. sum);

  <div className="max-w-lg mx-auto mt-2">
    <fieldset>
      <label htmlFor="income"> "Income"->React.string </label>
      <input
        className="bg-white focus:outline-none focus:shadow-outline border border-gray-300 rounded-lg py-2 px-4 block w-full appearance-none leading-normal"
        type_="number"
        placeholder="Your income..."
        id="income"
        onChange={event =>
          send(SetIncome(event->ReactEvent.Form.target##value))
        }
        value={state.income->Js.Float.toString}
      />
    </fieldset>
    <div
      className="py-2 px-3 my-6 border-top border-t-4 overflow-scroll"
      style={ReactDOMRe.Style.make(~height="calc(100vh - 200px)", ())}>
      <h4 className="text-lg mb-4 flex justify-between">
        <span> "Outcomes"->React.string </span>
        <button
          className="bg-blue-500 hover:bg-blue-700 text-white font-bold py-1 px-2 rounded text-xs"
          onClick={_ => send(ToggleCreationForm)}>
          "Add outcome"->React.string
        </button>
      </h4>
      {state.isCreating
         ? <OutcomeForm
             onCancel={_ => send(ToggleCreationForm)}
             onSubmit={outcome => send(OnCreationSubmit(outcome))}
           />
         : React.null}
      {state.outcome
       ->Array.mapWithIndex((index, outcome) =>
           <div
             key={outcome.id}
             className={
               "py-3 " ++ (index === 0 ? "" : "border-solid border-t")
             }>
             <OutcomeItem
               outcome
               onSubmit={outcome => send(UpdateOutcome(outcome))}
               onDelete={() => send(DeleteOutcome(outcome))}
             />
           </div>
         )
       ->React.array}
    </div>
    <div className="border-solid border-t-4 flex justify-between">
      <p className="text-lg"> "Remaining"->React.string </p>
      <p className="text-2xl">
        {remaining->Js.Float.toString->React.string}
      </p>
    </div>
  </div>;
};

let default = make;