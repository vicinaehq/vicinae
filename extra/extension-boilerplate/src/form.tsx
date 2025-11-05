import { Action, ActionPanel, Form, showToast, Toast } from "@vicinae/api";

// Examples taken from:
// https://developers.raycast.com/api-reference/user-interface/form

export default function FormElements() {
	return (
		<Form
			actions={
				<ActionPanel>
					<Action title="Submit" onAction={console.log} />
				</ActionPanel>
			}
			navigationTitle="Form Elements"
			searchBarAccessory={
				<Form.LinkAccessory
					target="https://developers.raycast.com/api-reference/user-interface/form"
					text="Open Documentation"
				/>
			}
		>
			{/* TextField */}
			<Form.TextField id="name" title="TextField" defaultValue="Steve" />

			{/* PasswordField */}
			<Form.PasswordField id="password" title="PasswordField" />

			{/* TextArea */}
			<Form.TextArea id="description" title="TextArea" />

			{/* Checkbox */}
			<Form.Checkbox
				id="answer"
				title="Checkbox"
				label="Are you happy?"
				defaultValue={true}
			/>

			{/* DatePicker */}
			<Form.DatePicker
				id="dateOfBirth"
				title="DatePicker"
				type={Form.DatePicker.Type.Date}
				defaultValue={new Date()}
			/>

			<Form.DatePicker
				id="datetime"
				title="DatePicker with time"
				type={Form.DatePicker.Type.DateTime}
				defaultValue={new Date()}
			/>

			{/* Dropdown */}
			<Form.Dropdown id="emoji" title="Dropdown" defaultValue="lol">
				<Form.Dropdown.Item value="poop" title="Pile of poop" icon="💩" />
				<Form.Dropdown.Item value="rocket" title="Rocket" icon="🚀" />
				<Form.Dropdown.Item
					value="lol"
					title="Rolling on the floor laughing face"
					icon="🤣"
				/>
			</Form.Dropdown>

			{/* Dropdown with sections */}
			<Form.Dropdown id="food" title="Dropdown with sections">
				<Form.Dropdown.Section title="Fruits">
					<Form.Dropdown.Item value="apple" title="Apple" icon="🍎" />
					<Form.Dropdown.Item value="banana" title="Banana" icon="🍌" />
				</Form.Dropdown.Section>
				<Form.Dropdown.Section title="Vegetables">
					<Form.Dropdown.Item value="broccoli" title="Broccoli" icon="🥦" />
					<Form.Dropdown.Item value="carrot" title="Carrot" icon="🥕" />
				</Form.Dropdown.Section>
			</Form.Dropdown>

			{/* TagPicker */}
			<Form.TagPicker id="sports" title="TagPicker" defaultValue={["football"]}>
				<Form.TagPicker.Item value="basketball" title="Basketball" icon="🏀" />
				<Form.TagPicker.Item value="football" title="Football" icon="⚽️" />
				<Form.TagPicker.Item value="tennis" title="Tennis" icon="🎾" />
			</Form.TagPicker>

			{/* Separator */}
			<Form.Separator />

			{/* FilePicker - Single File */}
			<Form.FilePicker
				id="singleFile"
				title="(single file)"
				onChange={(files) =>
					showToast(
						Toast.Style.Success,
						`Selected ${Array.isArray(files) ? files.length : files ? 1 : 0} file(s)`,
					)
				}
			/>

			{/* FilePicker - Multiple Files */}
			<Form.FilePicker
				id="multipleFiles"
				title="(multiple files)"
				allowMultipleSelection={true}
				onChange={(files) =>
					showToast(
						Toast.Style.Success,
						`Selected ${Array.isArray(files) ? files.length : files ? 1 : 0} file(s)`,
					)
				}
			/>

			{/* FilePicker - Single Directory */}
			<Form.FilePicker
				id="singleDirectory"
				title="(single directory)"
				canChooseDirectories={true}
				canChooseFiles={false}
				onChange={(files) =>
					showToast(
						Toast.Style.Success,
						`Selected ${Array.isArray(files) ? files.length : files ? 1 : 0} directory(ies)`,
					)
				}
			/>

			{/* FilePicker - Multiple Directories */}
			{/* Qt doesn't support multi-directory picking */}
			{/* <Form.FilePicker
				id="multipleDirectories"
				title="FilePicker (multiple directories)"
				allowMultipleSelection={true}
				canChooseDirectories={true}
				canChooseFiles={false}
				onChange={(files) =>
					showToast(
						Toast.Style.Success,
						`Selected ${Array.isArray(files) ? files.length : files ? 1 : 0} directory(ies)`,
					)
				}
			/> */}

			{/* FilePicker - With Hidden Files */}
			{/* TODO: filtering isn't implemented yet */}
			{/* <Form.FilePicker
				id="filesWithHidden"
				title="(with hidden files)"
				showHiddenFiles={true}
				onChange={(files) =>
					showToast(
						Toast.Style.Success,
						`Selected ${Array.isArray(files) ? files.length : files ? 1 : 0} file(s)`,
					)
				}
			/> */}

			{/* Description */}
			<Form.Description
				title="Description"
				text="This is a description. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua."
			/>
		</Form>
	);
}
