import {
	Action,
	ActionPanel,
	Form,
	List,
	showToast,
	Toast,
	useNavigation,
} from "@vicinae/api";

function NestedForm({ item }: { item: string }) {
	return (
		<Form
			navigationTitle={`Edit ${item}`}
			actions={
				<ActionPanel>
					<Action.SubmitForm
						title="Submit"
						onSubmit={(values) =>
							showToast(
								Toast.Style.Success,
								`Submitted: ${JSON.stringify(values)}`,
							)
						}
					/>
				</ActionPanel>
			}
		>
			<Form.TextField
				id="name"
				title="Name"
				placeholder="Enter a name..."
				defaultValue={item}
			/>
			<Form.PasswordField
				id="secret"
				title="Secret"
				placeholder="Enter a secret..."
			/>
			<Form.TextArea
				id="notes"
				title="Notes"
				placeholder="Enter some notes..."
			/>
			<Form.Checkbox id="agree" title="Confirm" label="I agree" />
			<Form.DatePicker id="date" title="Date" />
			<Form.Dropdown id="priority" title="Priority" defaultValue="medium">
				<Form.Dropdown.Item value="low" title="Low" />
				<Form.Dropdown.Item value="medium" title="Medium" />
				<Form.Dropdown.Item value="high" title="High" />
			</Form.Dropdown>
		</Form>
	);
}

export default function NestedFormTest() {
	const { push } = useNavigation();
	const items = ["Alpha", "Bravo", "Charlie", "Delta", "Echo"];

	return (
		<List searchBarPlaceholder="Pick an item to edit...">
			{items.map((item) => (
				<List.Item
					key={item}
					title={item}
					subtitle="Push to form"
					actions={
						<ActionPanel>
							<Action.Push
								title="Open Form"
								target={<NestedForm item={item} />}
							/>
						</ActionPanel>
					}
				/>
			))}
		</List>
	);
}
