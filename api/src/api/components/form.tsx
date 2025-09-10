import { ReactNode, Ref } from "react";
import { useImperativeFormHandle } from "../hooks/use-imperative-form-handle";
import { ImageLike, serializeImageLike } from "../image";

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

const TextField: React.FC<TextFieldProps> = ({
  ref,
  ...props
}) => {
  useImperativeFormHandle(ref);

  return (
    <text-field {...props} />
  );
};

interface PasswordFieldProps
  extends FormItemProps<string>,
    WithFormRef<Form.PasswordField> {}

const PasswordField: React.FC<PasswordFieldProps> = ({ ...props }) => {
  return <password-field {...props} />;
};

interface DatePickerProps
  extends FormItemProps<Date | null>,
    WithFormRef<Form.DatePicker> {}

const DatePicker: React.FC<DatePickerProps> = ({ ...props }) => {
  return <date-picker-field {...props} />;
};

interface CheckboxProps
  extends FormItemProps<boolean>,
    WithFormRef<Form.Checkbox> {
  label?: string;
}

const Checkbox: React.FC<CheckboxProps> = ({
  ref,
  ...props
}) => {
  useImperativeFormHandle(ref);

  return (
    <checkbox-field {...props} />
  );
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

const Dropdown: React.FC<DropdownProps> = ({
  children,
  ...props
}) => {
  return (
    <dropdown-field {...props}>
      {children}
    </dropdown-field>
  );
};

interface TagPickerProps
  extends FormItemProps<string[]>,
    WithFormRef<Form.TagPicker> {
  children?: ReactNode;
}

const TagPickerRoot: React.FC<TagPickerProps> = ({
  children,
  ...props
}) => {
  return (
    <tag-picker-field {...props}>
      {children}
    </tag-picker-field>
  );
};

const TagPickerItem: React.FC<{ title: string, value: string, icon: ImageLike }> = ({ icon, ...props }) => {
	return <tag-picker-item {...props} icon={icon && serializeImageLike(icon)}  />
}

const TagPicker = Object.assign(TagPickerRoot, {
	Item: TagPickerItem
});

interface TextAreaProps
  extends FormItemProps<string>,
    WithFormRef<Form.TagPicker> {
}

const TextArea: React.FC<TextAreaProps> = (props) => {
  return (
    <text-area-field {...props} />
  );
};

interface FilePickerProps extends FormItemProps<string[]>, WithFormRef<Form.FilePicker> {
}

const FilePicker: React.FC<FilePickerProps> = (props) => {
  return (
    <file-picker-field {...props} />
  );
};

type DescriptionProps = {
	text: string;
	title?: string;
};

const Description: React.FC<DescriptionProps> = (props) => {
	return <form-description {...props} />
}

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
});
