package recordblob

import "unsafe"

// RecordBlob is a struct that contains the record blob.  Having the record blob embedded in a Go struct prevents
// the unsafe package from being required in client code.
type RecordBlob struct {
	Blob unsafe.Pointer
}

func NewRecordBlob(record unsafe.Pointer) *RecordBlob {
	return &RecordBlob{Blob: record}
}