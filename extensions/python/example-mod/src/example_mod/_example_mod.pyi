import enum
from typing import Any, Generic, override, TypeVar

import bitarray

_PY_T = TypeVar("_PY_T")
_ASN_T = TypeVar("_ASN_T", bound="_Asn1Type")

# determined by compiler arguments
_ASN_BER = True
_ASN_XER = True
_ASN_JER = True
_ASN_TXT = True
_ASN_PER = True
_ASN_OER = True

# GENERATION OF STUBTS NOT IMPLEMENTED !!!

# ---
# The following types are just abstractions to describe the generated
# Python types. They are not used!
# ---

# Each type alias, enumerated or constructed type WILL generate its
# own class. Each class will implement the methods described in this
# base class (TYPING ONLY!).
# Conversions for default basic types (unless enumerated) will be
# performed inline (no default implementation necessary).
class _Asn1Type:
    # All generic types implement default repr() and str() behaviour. While
    # repr just prints out the class name, str() will print out the value
    # if present/set.
    @override
    def __repr__(self) -> str: ...
    @override
    def __str__(self) -> str: ...

    # Encoding and decoding is as simple as calling the two desired methods. The
    # generated methods will depend on the compiler arguments. Below is a full
    # list of all supported encoders/decoders. Note that the decode methods will
    # be implemented as STATIC methods. Type checkers require the use of the
    # classmethod to infer the right type.
    if _ASN_BER:
        # Basic Encoding Rules (BER)
        def ber_encode(self) -> bytes: ...
        @classmethod
        def ber_decode(cls: type[_ASN_T], data: bytes) -> _ASN_T: ...

        # Canonical Encoding Rules (CER)
        def cer_encode(self) -> bytes: ...
        @classmethod
        def cer_decode(cls: type[_ASN_T], data: bytes) -> _ASN_T: ...

        # Distinguished Encoding Rules (DER)
        def der_encode(self) -> bytes: ...
        @classmethod
        def der_decode(cls: type[_ASN_T], data: bytes) -> _ASN_T: ...

    if _ASN_XER:
        # XML Encoding Rules (XER)
        def xer_encode(self, /, *, canonical: bool = ...) -> bytes: ...
        @classmethod
        def xer_decode(
            cls: type[_ASN_T], data: bytes, /, *, canonical: bool = ...
        ) -> _ASN_T: ...

    if _ASN_JER:
        # JSON Encoding Rules (JER)
        def jer_encode(self, /, *, minified: bool = ...) -> bytes: ...
        @classmethod
        def jer_decode(
            cls: type[_ASN_T], data: bytes, /, *, minified: bool = ...
        ) -> _ASN_T: ...

    if _ASN_OER:
        # Octet Encoding Rules (OER)
        def oer_encode(self, /, *, canonical: bool = ...) -> bytes: ...
        @classmethod
        def oer_decode(
            cls: type[_ASN_T], data: bytes, /, *, canonical: bool = ...
        ) -> _ASN_T: ...

    if _ASN_PER:
        # Packed Encoding Rules (PER)
        def per_encode(
            self, /, *, canonical: bool = ..., aligned: bool = ...
        ) -> bytes: ...
        @classmethod
        def per_decode(
            cls: type[_ASN_T],
            data: bytes,
            /,
            *,
            canonical: bool = ...,
            aligned: bool = ...,
        ) -> _ASN_T: ...

    if _ASN_TXT:
        # Special method with a nonstandard textual output of this object
        def to_text(self) -> bytes: ...

    # To verify the internal value WITHOUT raising an exception, use
    # the following method
    def is_valid(self) -> bool: ...
    # to check and raise an exception if the value is not valid, use
    def check_constraints(self) -> None: ...

# Each generated BASIC type has its own class that conforms to the following
# abstract class:
class _Asn1BasicType(Generic[_PY_T], _Asn1Type):
    # Each basic type stores its value in a property called `value`. The current
    # ASN.1 type mapping is as follows:
    #   INTEGER      <-> int
    #   BOOLEAN      <-> bool
    #   REAL         <-> float
    #   NULL         <-> None
    #   BIT_STRING   <-> bitarray.bitarray (external)
    #   OCTET_STRING <-> bytes
    #   OID          <-> str
    #   UTF8_STRING  <-> str (all other string types)

    # Initialization of the target value is OPTIONAL
    def __init__(self, value: _PY_T = ...) -> None: ...
    @property
    def value(self) -> _PY_T: ...
    # It may be set with different python values - look out for the
    # documenation on each type.
    @value.setter
    def value(self, value: _PY_T) -> None: ...

# Same organization, but different conversion. Enumerated types store
# an additional enumeration that stores all defined named values.
class _Asn1EnumType(_Asn1Type):
    # The class will ALWAYS be named "VALUES". members prefixed with 'V_'
    class VALUES(enum.IntEnum): ...

    # Each basic type stores its value in a property called `value`
    @property
    def value(self) -> _Asn1EnumType.VALUES: ...
    # Here, we can use the Python value assigned to each enum member
    # or the enum member itself
    @value.setter
    def value(self, value: _Asn1EnumType.VALUES | int) -> None: ...

# A special case for named BIT STRING definitions will store a custom
# enum type ("VALUES") as well, but the internal value uses a bitarray.
# The internal value-to-bitarray conversion uses LITTLE ENDIAN aligned
# bitarrays.
class _Asn1FlagType(_Asn1Type):
    # The class uses the builtin enum.IntFlag to allow multiple values
    # to be set. Members are prefixed with 'V_'
    class VALUES(enum.IntFlag): ...

    # Each basic type stores its value in a property called `value`
    @property
    def value(self) -> _Asn1FlagType.VALUES: ...

    # Allowed value types:
    #   int - enum value (a instance of VALUES can be used as well as it inherits from int)
    #   bytes - directly initialized the underlying BIT STRING
    #   bitarray - will be converted to bytes and then copied
    @value.setter
    def value(
        self,
        value: _Asn1FlagType.VALUES | int | bytes | bitarray.bitarray,
    ) -> None: ...

# -- CHOICE
# Each union type /CHOICE class implements a special behaviour as only
# one value can be present at a time. To keep track of the currently stored
# representation, an enumeration is generated ("PRESENT"). Each member will
# be associated to a specific value of the enumeration.
class _Asn1_ChoiceType(_Asn1Type):
    # all possible states
    class PRESENT(enum.IntEnum):
        # By default, the invalid state is marked by a value of 0
        PR_NOTHING = 0
        # naming scheme is as follows:
        #   - <name> := PR_<member_name>
        ...

    # -- members... --
    # Mambers can be set using keyword arguments only!
    def __init__(self, /, **members: Any) -> None: ...

# -- PROPOSED --
# asn1c should optionally generaty a stub file for each type based in the
# skeleton code above. For instance:
class Signed8(_Asn1BasicType[int]):
    pass

class NamedSigned8(_Asn1Type):
    class VALUES(enum.Enum):
        V_first = -1
        V_second = 0
        V_third = 1

    @property
    def value(self) -> NamedSigned8.VALUES: ...
    @value.setter
    def value(self, value: NamedSigned8.VALUES | int) -> None: ...

class ExampleChoice(_Asn1Type):
    foo: bytes | None
    bar: int | None
    baz: NamedSigned8 | None

class ExampleNull(_Asn1BasicType[None]):
    pass
