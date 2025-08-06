import enum
from typing import Generic, override, TypeVar

import bitarray

_PY_T = TypeVar("_PY_T")

# GENERATION NOT IMPLEMENTED
# Each type alias, enumerated or constructed type WILL generate its
# own class. Each class will implement the methods described in this
# base class (TYPING ONLY!).
# Conversions for default basic types (unless enumerated) will be
# performed inline (no default implementation necessary).
class _Asn1ABC:
    # Initialization of the target value is OPTIONAL
    def __init__(self, value: _PY_T = ...) -> None: ...
    # All generic types implement default repr() and str() behaviour. While
    # repr just prints out the class name, str() will print out the value
    # if present/set.
    @override
    def __repr__(self) -> str: ...
    @override
    def __str__(self) -> str: ...

    # Encoding and decoding is as simple as calling two methods. The
    # internal binding will do the rest and take care of the encoding.
    def encode(self) -> bytes: ...
    # @staticmethod
    # def decode(data: bytes) -> ClsType: ...
    # To verify the internal value WITHOUT raising an exception, use
    # the following method
    def is_valid(self) -> bool: ...
    # to check and raise an exception if the value is not valid, use
    def check_constraints(self) -> None: ...

# Each generated BASIC type has its own class that conforms to the following
# abstract class:
class _BasicAsn1Type(Generic[_PY_T], _Asn1ABC):
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
    @property
    def value(self) -> _PY_T: ...
    # It may be set with different python values - look out for the
    # documenation on each type.
    @value.setter
    def value(self, value: _PY_T) -> None: ...

# Same organization, but different conversion. Enumerated types store
# an additional enumeration that stores all defined named values.
class _BasicAsn1EnumType(_Asn1ABC):
    # The class will ALWAYS be named "VALUES".
    class VALUES(enum.Enum): ...

    # Each basic type stores its value in a property called `value`
    @property
    def value(self) -> _BasicAsn1EnumType.VALUES: ...
    # Here, we can use the Python value assigned to each enum member
    # or the enum member itself
    @value.setter
    def value(self, value: _BasicAsn1EnumType.VALUES | int) -> None: ...

    # decode will return an instance of this class
    @staticmethod
    def decode(data: bytes) -> _BasicAsn1EnumType: ...

# A special case for named BIT STRING definitions will store a custom
# enum type ("VALUES") as well, but the internal value uses a bitarray.
# The internal value-to-bitarray conversion uses LITTLE ENDIAN aligned
# bitarrays.
class _BasicAsn1FlagType(_Asn1ABC):
    # The class uses the builtin enum.IntFlag to allow multiple values
    # to be set.
    class VALUES(enum.IntFlag): ...

    # Each basic type stores its value in a property called `value`
    @property
    def value(self) -> _BasicAsn1FlagType.VALUES: ...

    # Allowed value types:
    #   int - enum value (a instance of VALUES can be used as well as it inherits from int)
    #   bytes - directly initialized the underlying BIT STRING
    #   bitarray - will be converted to bytes and then copied
    @value.setter
    def value(
        self,
        value: _BasicAsn1FlagType.VALUES | int | bytes | bitarray.bitarray,
    ) -> None: ...

    # decode will return an instance of this class
    @staticmethod
    def decode(data: bytes) -> _BasicAsn1FlagType: ...

# -- PROPOSED --
# asn1c should optionally generaty a stub file for each type based in the
# skeleton code above. For instance:
class Signed8(_BasicAsn1Type[int]):
    @staticmethod
    def decode(data: bytes) -> Signed8: ...

class NamedSigned8(_Asn1ABC):
    class VALUES(enum.Enum):
        NamedSigned8_first = -1
        NamedSigned8_second = 0
        NamedSigned8_third = 1

    @property
    def value(self) -> NamedSigned8.VALUES: ...
    @value.setter
    def value(self, value: NamedSigned8.VALUES | int) -> None: ...
    @staticmethod
    def decode(data: bytes) -> NamedSigned8: ...

class ExampleChoice(_Asn1ABC):
    foo: bytes | None
    bar: int | None
    baz: NamedSigned8 | None

    @staticmethod
    def decode(data: bytes) -> ExampleChoice: ...
