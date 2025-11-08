import { ReactNode, Ref } from "react";
import { useImperativeFormHandle } from "../hooks/use-imperative-form-handle";
import { type ImageLike, serializeProtoImage } from "../image";
import { Dropdown as MainDropdown } from "./dropdown";

type FormProps = {
	actions?: React.ReactNode;
	children?: React.ReactNode;
	enableDrafts?: boolean;
	isLoading?: boolean;
	navigationTitle?: string;
	searchBarAccessory?: React.ReactNode;
};

export type FormItemRef = {
	focus: () => void;
	reset: () => void;
};

interface FormItemProps<T extends Form.Value> {
	id: string;
	title?: string;
	info?: string;
	error?: string;
	storeValue?: boolean;
	autoFocus?: boolean;
	value?: T;
	defaultValue?: T;
	onChange?: (newValue: T) => void;
	onBlur?: (event: FormEvent<T>) => void;
	onFocus?: (event: FormEvent<T>) => void;
}

interface FormEvent<T extends Form.Value> {
	target: {
		id: string;
		value?: T;
	};
	type: FormEventType;
}

type FormEventType = "focus" | "blur";

export declare namespace Form {
	export type Props = FormProps;
	export type TextField = FormItemRef;
	export type PasswordField = FormItemRef;
	export type TextArea = FormItemRef;
	export type Checkbox = FormItemRef;
	export type DatePicker = FormItemRef;
	export type Dropdown = FormItemRef;
	export type TagPicker = FormItemRef;
	export type FilePicker = FormItemRef;

	export type ItemReference = FormItemRef;

	export type ItemProps<T extends Value> = FormItemProps<T>;

	export type Value =
		| string
		| number
		| boolean
		| string[]
		| number[]
		| Date
		| null;

	export type Values = {
		[itemId: string]: Value;
	};
}

const wrapFormItemProps = <T extends Form.Value>(props: FormItemProps<T>) => {
	return {
		...props,
		onFocus: () =>
			props.onFocus?.({
				type: "focus",
				target: {
					id: props.id,
					value: (props.value ?? props.defaultValue) as T | undefined,
				},
			}),
		onBlur: () =>
			props.onBlur?.({
				type: "blur",
				target: {
					id: props.id,
					value: (props.value ?? props.defaultValue) as T | undefined,
				},
			}),
	};
};

const FormRoot: React.FC<Form.Props> = ({
	enableDrafts = false,
	actions,
	children,
	isLoading = false,
	navigationTitle,
	searchBarAccessory,
}) => {
	return (
		<form
			//@ts-ignore
			enableDrafts={enableDrafts}
			isLoading={isLoading}
			navigationTitle={navigationTitle}
		>
			{searchBarAccessory}
			{children}
			{actions}
		</form>
	);
};

interface WithFormRef<T> {
	ref?: Ref<T>;
}

interface TextFieldProps
	extends FormItemProps<string>,
		WithFormRef<Form.TextField> {}

const TextField: React.FC<TextFieldProps> = ({ ref, ...props }) => {
	useImperativeFormHandle(ref);

	return <text-field {...wrapFormItemProps(props)} />;
};

interface PasswordFieldProps
	extends FormItemProps<string>,
		WithFormRef<Form.PasswordField> {}

const PasswordField: React.FC<PasswordFieldProps> = ({ ref, ...props }) => {
	useImperativeFormHandle(ref);

	return <password-field {...props} />;
};

export enum DatePickerType {
	DateTime = "dateTime",
	Date = "date",
}

interface DatePickerProps
	extends FormItemProps<Date | null>,
		WithFormRef<Form.DatePicker> {
	min?: Date;
	max?: Date;
	type?: DatePickerType;
}

const DatePickerRoot: React.FC<DatePickerProps> = ({
	ref,
	onChange,
	...props
}) => {
	useImperativeFormHandle(ref);

	const _onChange = onChange
		? (newValue: string | null) => {
				const dateObj = newValue ? new Date(newValue) : null;
				onChange(dateObj);
			}
		: undefined;

	return (
		<date-picker-field {...wrapFormItemProps(props)} onChange={_onChange} />
	);
};

const DatePicker = Object.assign(DatePickerRoot, {
	Type: DatePickerType,
	isFullDay: (value: Date | null | undefined) => {
		if (!value) return false;
		return (
			value.getHours() === 0 &&
			value.getMinutes() === 0 &&
			value.getSeconds() === 0 &&
			value.getMilliseconds() === 0
		);
	},
});

interface CheckboxProps
	extends FormItemProps<boolean>,
		WithFormRef<Form.Checkbox> {
	label?: string;
}

const Checkbox: React.FC<CheckboxProps> = ({ ref, ...props }) => {
	useImperativeFormHandle(ref);

	return <checkbox-field {...wrapFormItemProps(props)} />;
};

interface DropdownProps
	extends FormItemProps<string>,
		WithFormRef<Form.Dropdown> {
	tooltip?: string;
	children?: ReactNode;
	filtering?: boolean;
	isLoading?: boolean;
	placeholder?: string;
	throttle?: boolean;
	onSearchTextChange?: (text: string) => void;
}

//FIXME: we probably need to reuse the existing dropdown in
// a smarter way.
const DropdownRoot: React.FC<DropdownProps> = ({ ref, children, ...props }) => {
	// FIXME: testing stuff, we need to generalize this to all form items
	useImperativeFormHandle(ref);

	return (
		<dropdown-field {...wrapFormItemProps(props)}>{children}</dropdown-field>
	);
};

const Dropdown = Object.assign(DropdownRoot, {
	Item: MainDropdown.Item,
	Section: MainDropdown.Section,
});

interface TagPickerProps
	extends FormItemProps<string[]>,
		WithFormRef<Form.TagPicker> {
	children?: ReactNode;
}

const TagPickerRoot: React.FC<TagPickerProps> = ({ children, ...props }) => {
	return (
		<tag-picker-field {...wrapFormItemProps(props)}>
			{children}
		</tag-picker-field>
	);
};

const TagPickerItem: React.FC<{
	title: string;
	value: string;
	icon: ImageLike;
}> = ({ icon, ...props }) => {
	const serializedIcon = icon ? serializeProtoImage(icon) : icon;
	return <tag-picker-item {...props} icon={serializedIcon} />;
};

const TagPicker = Object.assign(TagPickerRoot, {
	Item: TagPickerItem,
});

interface TextAreaProps
	extends FormItemProps<string>,
		WithFormRef<Form.TextArea> {}

const TextArea: React.FC<TextAreaProps> = ({ ref, ...props }) => {
	useImperativeFormHandle(ref);

	return <text-area-field {...wrapFormItemProps(props)} />;
};

interface FilePickerProps
	extends FormItemProps<string[]>,
		WithFormRef<Form.FilePicker> {
	allowMultipleSelection?: boolean;
	canChooseDirectories?: boolean;
	canChooseFiles?: boolean;
	showHiddenFiles?: boolean;
}

const FilePicker: React.FC<FilePickerProps> = ({ ref, ...props }) => {
	useImperativeFormHandle(ref);

	return <file-picker-field {...wrapFormItemProps(props)} />;
};

type DescriptionProps = {
	text: string;
	title?: string;
};

const Description: React.FC<DescriptionProps> = (props) => {
	return <form-description {...props} />;
};

export const Form = Object.assign(FormRoot, {
	TextField,
	PasswordField,
	DatePicker,
	Checkbox,
	TextArea,
	Dropdown,
	Description,
	TagPicker,
	FilePicker,
	Separator: () => <separator />,
	LinkAccessory: ({ target, text }: { target: string; text: string }) => (
		<link-accessory target={target} text={text} />
	),
});
