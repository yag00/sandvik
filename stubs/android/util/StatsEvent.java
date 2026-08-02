package android.util;

public final class StatsEvent {
    private StatsEvent() {
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Builder {
        public Builder() {
        }

        public Builder setAtomId(int atomId) {
            return this;
        }

        public Builder writeInt(int value) {
            return this;
        }

        public Builder writeLong(long value) {
            return this;
        }

        public Builder writeBoolean(boolean value) {
            return this;
        }

        public Builder writeString(String value) {
            return this;
        }

        public Builder writeByteArray(byte[] value) {
            return this;
        }

        public Builder writeIntArray(int[] value) {
            return this;
        }

        public Builder writeLongArray(long[] value) {
            return this;
        }

        public Builder writeStringArray(String[] value) {
            return this;
        }

        public Builder addBooleanAnnotation(byte annotationId, boolean value) {
            return this;
        }

        public Builder usePooledBuffer() {
            return this;
        }

        public StatsEvent build() {
            return new StatsEvent();
        }
    }
}
