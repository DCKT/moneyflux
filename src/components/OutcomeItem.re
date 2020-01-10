[@react.component]
let make = (~outcome: Outcome.t, ~onSubmit, ~onDelete) => {
  let (isEditing, setIsEditing) = React.useState(() => false);

  switch (isEditing) {
  | false =>
    <div className="flex items-center">
      <button
        className="mr-2 hover:bg-red-600 hover:text-white hover:border-red-600 border border-solid rounded p-1"
        onClick={_ => onDelete()}>
        {j|✕|j}->React.string
      </button>
      <button
        className="mr-3 hover:bg-gray-300 border border-solid rounded p-1"
        onClick={_ => setIsEditing(_ => true)}>
        {j|✎|j}->React.string
      </button>
      <small> outcome.label->React.string </small>
      <strong className="ml-auto">
        {outcome.price->Js.Float.toString->React.string}
      </strong>
    </div>
  | true =>
    <OutcomeForm
      initialValue=outcome
      onSubmit={outcome => {
        onSubmit(outcome);
        setIsEditing(_ => false);
      }}
      onCancel={_ => setIsEditing(_ => false)}
    />
  };
};